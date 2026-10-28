#include "EventManager.h"
#include <vector>
#include <algorithm>
#include "Event.h"
#include "IEventSubscriber.h"
#include "common/Common.h"
#include "graphic/RectDebugging.h"
#include <file/SettingsManager.h>
#include <sstream>
#include <string>
#include "EventFactory.h"
#include "UpdateAllGameObjectsEvent.h"

using namespace std;

namespace gamelib
{	
	EventManager::EventManager() : logEvents(false), printStatistics(false)
	{
	}

	std::map<const EventId, std::vector<IEventSubscriber*>> EventManager::GetSubscriptions()
	{
		std::scoped_lock lock(mutex);

		return eventSubscribers;
	}

	std::string EventManager::GetSubscriberName()
	{
		return "EventManager";
	}

	void EventManager::ClearSubscribers()
	{
		{
			std::scoped_lock lock(mutex);
			this->eventSubscribers.clear();
		}
	}

	EventManager* EventManager::Get()
	{
		static EventManager instance;
		return &instance;
	}

	size_t EventManager::CountReady() const
	{
		std::scoped_lock lock(mutex);
		return primaryEventQueue.size() + secondaryEventQueue.size();
	}

	std::queue<shared_ptr<Event>> EventManager::GetEvents()
	{
		std::scoped_lock lock(mutex);
		return primaryEventQueue;
	}

	void EventManager::Reset()
	{
		{
			std::scoped_lock lock(mutex);

			std::queue<shared_ptr<Event>> empty;
			std::swap(primaryEventQueue, empty);
			std::swap(secondaryEventQueue, empty);
		}
	}

	EventManager::~EventManager()
	{
		Logger::Get()->LogThis("Event manager dying.");
	}

	
	bool EventManager::Initialize() 
	{ 
		logEvents = SettingsManager::Get()->GetBool("eventManager", "logEvents");
		return true;
	}

	void EventManager::RaiseEvent(const shared_ptr<Event>& event, IEventSubscriber* you)
	{	
		if(event == nullptr) { THROW(0, "Can't raise a null or empty event", "EventManager");}
		if(!you) { Logger::Get()->LogThis("Invalid sender", true); return; }
		
		event->Origin = you->GetSubscriberName();
		
		{
			std::scoped_lock lock(mutex);
			primaryEventQueue.push(event);
		}

		LogEventRaised(you, event);
	}

	void EventManager::LogEventRaised(IEventSubscriber* you, const std::shared_ptr<Event>& event) const
	{
		if (logEvents)
		{
			std::stringstream log;
			log << "EventManager: " << you->GetSubscriberName() << " raised to event " << event->Id.Name;

			if (event->Id.PrimaryId != UpdateAllGameObjectsEventTypeEventId.PrimaryId) { Logger::Get()->LogThis(log.str()); }
		}
	}

	void EventManager::RaiseEventWithNoLogging(const std::shared_ptr<Event>& event)
	{
		std::scoped_lock lock(mutex);

		primaryEventQueue.push(event);
	}

	void EventManager::LogEventSubscription(const EventId& eventId, IEventSubscriber* pYou) const
	{
		if (logEvents) 
		{
			std::stringstream message;
			message << "EventManager: " << pYou->GetSubscriberName() << " subscribed to event " << eventId.Name;

			Logger::Get()->LogThis(message.str());
		}
	}

	void EventManager::SubscribeToEvent(const EventId& eventId, IEventSubscriber* pYou)
	{
		if (!pYou) return;

		LogEventSubscription(eventId, pYou);

		std::scoped_lock lock(mutex);

		auto& subscribers = eventSubscribers[eventId];
		if (std::find(subscribers.begin(), subscribers.end(), pYou) != subscribers.end())
		{
			return;
		}

		subscribers.push_back(pYou);
	}

	void EventManager::Send(const shared_ptr<Event>& event, IEventSubscriber* pSubscriber, const unsigned long deltaMs)
	{
		for(const auto &secondaryEvent : pSubscriber->HandleEvent(event, deltaMs))
		{
			AddToSecondaryEventQueue(secondaryEvent, pSubscriber);
		}

		// let subscribers tap/peek into events
		if(tap) 
			tap(event, pSubscriber);
	}

	void EventManager::SetEventTap(const std::function<void(const std::shared_ptr<Event>& event, const IEventSubscriber* pSubscriber)>& tapFn)
	{
		tap = tapFn;
	}

	/// <summary>
	/// Send event to each subscriber who has subscribed to that event.
	/// The subscriber does the work.
	/// </summary>
	/// <param name="event">Event to send to subscribers</param>
	/// <param name="deltaMs">delta time</param>
	void EventManager::DispatchEventToSubscriber(const shared_ptr<Event>& event, unsigned long deltaMs)
	{
		// Find the subscribers of this event
		std::vector<IEventSubscriber*> localSubscribers;
		{
			std::scoped_lock lock(mutex);
			auto it = eventSubscribers.find(event->Id);
			if (it != eventSubscribers.end())
			{
				localSubscribers = it->second;   // copy only the vector
			}
		}

		for (auto* pSubscriber : localSubscribers)
		{
			if (!pSubscriber)
			{
				std::scoped_lock lock(mutex);
				badSubscribersDuringDispatch++;
				continue;
			}

			Send(event, pSubscriber, deltaMs);

			{
				std::scoped_lock lock(mutex);
				eventsDispatched[event->Id]++;
			}
		}

		event->Processed = true;

		if (printStatistics)
		{
			std::scoped_lock lock(mutex);
			elapsedTimeMs += deltaMs;
			dispatchCalledTimes++;
			if (std::isgreater(elapsedTimeMs, 1000))
			{
				int totalPrimaryEvents = 0;
				std::stringstream str;
				for (const auto& [eventId, count] : eventsDispatched)
				{
					str << eventId.Name << " " << count << "/s ";
					totalPrimaryEvents += count;
				}
				std::cout << totalPrimaryEvents << " events/s over " << dispatchCalledTimes
					<< " dispatches. " << str.str() << '\n';

				eventsDispatched.clear();
				elapsedTimeMs = 0;
				noSubscribersDuringDispatch = badSubscribersDuringDispatch = dispatchCalledTimes = 0;
			}
		}
	}

	void EventManager::AddToSecondaryEventQueue(const std::shared_ptr<Event>& secondaryEvent, IEventSubscriber* originSubscriber)
	{
		// any results from processing are put onto the secondary queue
		secondaryEvent->Origin = originSubscriber->GetSubscriberName();

		{
			std::scoped_lock lock(mutex);
			secondaryEventQueue.push(secondaryEvent);
		}
	}

	void EventManager::DispatchEventToSubscriber(const shared_ptr<Event>& event, const std::string& target)
	{
		std::vector<IEventSubscriber*> local;
		
		{
			std::scoped_lock lock(mutex);
			local = eventSubscribers[event->Id];
		}


		if (local.empty())
		{
			event->Processed = true;
			return;
		} // if reset()

		// Go through each subscriber of the event and have the subscriber handle it
		for (const auto& pSubscriber : local)
		{
			if (!pSubscriber) { continue; }
			if (pSubscriber->GetSubscriberName() != target) { continue; }
						
			// allow subscriber to process the event
			Send(event, pSubscriber);
			event->Processed = true;
			return;
		}
		event->Processed = true;
	}

	void EventManager::ProcessAllEvents(const unsigned long deltaMs)
	{			
		std::queue<std::shared_ptr<Event>> local_primaryEventQueue; // swap to this so no others have access to it while processing.
		{
			std::scoped_lock lock(mutex);
			std::swap(local_primaryEventQueue, primaryEventQueue);
		}

		std::queue<std::shared_ptr<Event>> local_secondaryEventQueue; // swap to this so no others have access to it while processing.

		{
			std::scoped_lock lock(mutex);
			std::swap(local_secondaryEventQueue, secondaryEventQueue);
		}

		while(!local_primaryEventQueue.empty())
		{
			const auto& event = local_primaryEventQueue.front();
							
			if (event->Processed)
			{
				local_primaryEventQueue.pop();
				continue;
			}
			
			// Ask each subscriber to deal with event			
			DispatchEventToSubscriber(event, deltaMs);

			if (!local_primaryEventQueue.empty()) { local_primaryEventQueue.pop(); }
		}

		{
			std::scoped_lock lock(mutex);

			// Process the secondary queue once primary queue is processed
			while (!local_secondaryEventQueue.empty())
			{
				// put the secondary queue onto the back of the the primary queue for next cycle of processing

				primaryEventQueue.push(local_secondaryEventQueue.front());
				local_secondaryEventQueue.pop();
			}
		}
	}
	
	/// <summary>
	/// Handles events that the event manager subscribes to itself
	/// </summary>
	std::vector<std::shared_ptr<Event>> EventManager::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{
		// Does not handle any events in the event queue itself
		return {};
	}

	static bool IsSameSubscriber(IEventSubscriber* candidate, const int subscriptionId) 
	{
		if(candidate)
		{
			const auto foundMatch = candidate == nullptr 
				                         ? true 
				                         : candidate->GetSubscriberId() == subscriptionId;
			return foundMatch;
		}
		return false;
	};
		
	void EventManager::RemoveEventSubscription(int subscriptionId, const EventId& id)
	{
		std::scoped_lock lock(mutex);

		for (auto& [eventId, subscribers] : eventSubscribers)
		{
			if (eventId != id) continue;

			auto matchFn = [&](IEventSubscriber* candidate) { return IsSameSubscriber(candidate, subscriptionId); };
			auto it = std::find_if(begin(subscribers), end(subscribers), matchFn);
			if (it != end(subscribers))
			{
				subscribers.erase(it);
			}
		}
	}


	void EventManager::Unsubscribe(const int subscriptionId)
	{
		std::scoped_lock lock(mutex);

		for(auto & [eventId, subscribers] : eventSubscribers )
		{
			auto matchFn = [&](IEventSubscriber* candidate) { return IsSameSubscriber(candidate, subscriptionId); };

			// Look for the subscriber in list of subscribers for this event type
			auto findResult = std::find_if(begin(subscribers), end(subscribers), matchFn);

			if(findResult != std::end(subscribers))
			{
				subscribers.erase(findResult);				
			}
		}
	}
}
