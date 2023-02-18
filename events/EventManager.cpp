#include "EventManager.h"
#include <vector>
#include <algorithm>
#include "Event.h"
#include "IEventSubscriber.h"
#include "common/Common.h"
#include "util/RectDebugging.h"
#include <util/SettingsManager.h>
#include <sstream>
#include <string>
#include "UpdateAllGameObjectsEvent.h"

using namespace std;
namespace gamelib
{	
	EventManager::EventManager() : logEvents(false) { 	}

	std::map<const EventId, std::vector<IEventSubscriber*>>& EventManager::GetSubscriptions() { return eventSubscribers; }
	std::string EventManager::GetSubscriberName() { return "EventManager"; }
	void EventManager::ClearSubscribers() { this->eventSubscribers.clear(); }

	EventManager* EventManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new EventManager();
		}
		return instance;
	}

	EventManager* EventManager::instance = nullptr;

	size_t EventManager::CountReady() const { return primaryEventQueue.size() + secondaryEventQueue.size(); }

	void EventManager::Reset()
	{
		std::queue<shared_ptr<Event>> empty;
		std::swap( primaryEventQueue, empty );
		std::swap( secondaryEventQueue, empty );		  
	}

	EventManager::~EventManager() { Logger::Get()->LogThis("Event manager dying."); instance = nullptr; }

	
	bool EventManager::Initialize() 
	{ 
		logEvents = SettingsManager::Get()->GetBool("EventManager", "logEvents");
		return true;
	}

	void EventManager::RaiseEvent(const shared_ptr<Event> event, IEventSubscriber* you)  // NOLINT(performance-unnecessary-value-param)
	{	
		if(event == nullptr) { THROW(0, "Can't raise a null or empty event", "EventManager")}
		if(!you) { Logger::Get()->LogThis("Invalid sender", true); return; }
		
		event->Origin = you->GetSubscriberName();
		primaryEventQueue.push(event);		

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

	void EventManager::RaiseEventWithNoLogging(const std::shared_ptr<Event>& event) { primaryEventQueue.push(event); }
		
	void EventManager::SubscribeToEvent(const EventId& eventId, IEventSubscriber* you)
	{
		if(you)
		{
			std::stringstream message;
			message << "EventManager: " << you->GetSubscriberName() << " subscribed to event " << eventId.Name;

			Logger::Get()->LogThis(message.str());
		
			eventSubscribers[eventId].push_back(you);
		}
	}

	/// <summary>
	/// Send event to each subscriber who has subscribed to that event.
	/// The subscriber does the work.
	/// </summary>
	/// <param name="event">Event to send to subscribers</param>
	void EventManager::DispatchEventToSubscriber(const shared_ptr<Event>& event, const unsigned long deltaMs)
	{
		// Go through each subscriber of the event and have the subscriber handle it
		for (const auto& pSubscriber :  eventSubscribers[event->Id])
		{
			if (eventSubscribers.empty()) { return; } 
			if (!pSubscriber) { continue; }
			
			// allow subscriber to process the event
			for(const auto &secondary_event : pSubscriber->HandleEvent(event, deltaMs))
			{
				AddToSecondaryEventQueue(secondary_event, pSubscriber);
			}	
			
		}
		event->Processed = true;
	}

	void EventManager::AddToSecondaryEventQueue(const std::shared_ptr<Event>& secondaryEvent, IEventSubscriber* originSubscriber)
	{
		// any results from processing are put onto the secondary queue
		secondaryEvent->Origin = originSubscriber->GetSubscriberName();
		secondaryEventQueue.push(secondaryEvent);
	}

	void EventManager::DispatchEventToSubscriber(const shared_ptr<Event>& event, const std::string& target)
	{
		// Go through each subscriber of the event and have the subscriber handle it
		for (const auto& pSubscriber :  eventSubscribers[event->Id])
		{
			if (eventSubscribers.empty()) { return; } // if reset()
			if (!pSubscriber) { continue; }
			if (pSubscriber->GetSubscriberName() != target) { return; }
						
			// allow subscriber to process the event
			for(const auto &secondary_event : pSubscriber->HandleEvent(event))
			{
				AddToSecondaryEventQueue(secondary_event, pSubscriber);
			}	
			
		}
		event->Processed = true;
	}

	void EventManager::ProcessAllEvents(const unsigned long deltaMs)
	{			
		while(!primaryEventQueue.empty())
		{
			const auto& event = primaryEventQueue.front();
							
			if (event->Processed) { continue; }
			
			// Ask each subscriber to deal with event			
			DispatchEventToSubscriber(event, deltaMs);

			if (!primaryEventQueue.empty()) { primaryEventQueue.pop(); }
		}

		// Process the secondary queue once primary queue is processed
		while(!secondaryEventQueue.empty())
		{
			// put the secondary queue onto the back of the the primary queue for next cycle of processing
			primaryEventQueue.push(secondaryEventQueue.front());
			secondaryEventQueue.pop();
		}
	}



	/// <summary>
	/// Handles events that the event manager subscribes to itself
	/// </summary>
	/// <param name="evt"></param>
	/// <param name="deltaMs"></param>
	/// <returns></returns>
	std::vector<std::shared_ptr<Event>> EventManager::HandleEvent(std::shared_ptr<Event> evt, unsigned long deltaMs)
	{
		// Does not handle any events in the event queue itself
		std::vector<std::shared_ptr<Event>> generated_events;
		return generated_events;
	}

	bool IsSameSubscriber(IEventSubscriber* candidate, const int subscriptionId) 
	{
		if(candidate)
		{
			const auto found_match = candidate == nullptr 
				                         ? true 
				                         : candidate->GetSubscriberId() == subscriptionId;
			return found_match;
		}
		return false;
	};
		
	void EventManager::RemoveEventSubscription(const int subscriptionId, const EventId& id)
	{
		for(auto & [eventId, subscribers] : eventSubscribers )
		{

			// We only remove subscriptions for the specific event type provided
			if(eventId != id) { continue; }

			// The subscriber's id must match what we have in our subscriptions
			auto matchFn = [&](IEventSubscriber* candidate) { return IsSameSubscriber(candidate, subscriptionId); };

			// Look for the subscriber in list of subscribers for this event type
			auto findResult = std::find_if(begin(subscribers), end(subscribers), matchFn);
			
			// Remove if found
			if(findResult != std::end(subscribers)) { subscribers.erase(findResult); }
		}		
	}

	void EventManager::Unsubscribe(const int subscriptionId)
	{
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

