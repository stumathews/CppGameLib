#include "EventManager.h"
#include <vector>
#include <algorithm>
#include "Event.h"
#include "IEventSubscriber.h"
#include "common/Common.h"
#include "util/RectDebugging.h"
#include <events/GameObjectEvent.h>
#include <util/SettingsManager.h>
#include <sstream>
#include <string>

using namespace std;
namespace gamelib
{

	
	EventManager::EventManager() { 	}

	EventManager* EventManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new EventManager();
		}
		return Instance;
	}

	EventManager* EventManager::Instance = nullptr;

	size_t EventManager::count_ready() const
	{
		return primary_event_queue_.size() + secondary_event_queue_.size();
	}

	void EventManager::reset()
	{
		std::queue<shared_ptr<Event>> empty;
		std::swap( primary_event_queue_, empty );
		std::swap( secondary_event_queue_, empty );		  
	}

	EventManager::~EventManager()
	{
		Logger::Get()->LogThis("Event manager dying.");
		Instance = nullptr;
	}

	
	bool EventManager::Initialize() 
	{ 
		logEvents = SettingsManager::Get()->GetBool("EventManager", "logEvents");
		return true;
	}

	void EventManager::ClearSubscribers()
	{
		this->event_subscribers_.clear();
	}
		
	void EventManager::RaiseEvent(const shared_ptr<Event> event, IEventSubscriber* you)  // NOLINT(performance-unnecessary-value-param)
	{	
		if(event == nullptr)
		{
			THROW(0, "Can't raise a null or empty event", "EventManager");
			return;
		}

		if(!you)
		{
			Logger::Get()->LogThis("Invalid sender", true);
			return;
		}

		if(logEvents)
		{
			std::stringstream log;
			log << "EventManager: " << you->GetSubscriberName()  << " raised to event " << ToString(event->type);
		
			if(event->type != EventType::DoLogicUpdateEventType)
			{
				Logger::Get()->LogThis(log.str());
			}
		}

		primary_event_queue_.push(event);		
	}

	void EventManager::RaiseEventWithNoLogging(const std::shared_ptr<Event> event)
	{
			primary_event_queue_.push(event);	
	
	}
		
	void EventManager::SubscribeToEvent(const EventType type, IEventSubscriber* you)
	{
		if(you)
		{
			std::stringstream message;
			message << "EventManager: " << you->GetSubscriberName() << " subscribed to event " << ToString(type);

			Logger::Get()->LogThis(message.str());
		
			event_subscribers_[type].push_back(you);
		}
	}

	/// <summary>
	/// Send event to each subscriber who has subscribed to that event.
	/// The subscriber does the work.
	/// </summary>
	/// <param name="event">Event to send to subscribers</param>
	void EventManager::DispatchEventToSubscriber(const shared_ptr<Event>& event)
	{
		// Go through each subscriber of the event and have the subscriber handle it
		for (const auto& pSubscriber :  event_subscribers_[event->type])
		{
			if(event_subscribers_.empty()) return; // if reset()
			
			// Check if the subscriber pointer is valid
			if(pSubscriber)
			{
				// allow subscriber to process the event
				for(const auto &secondary_event : pSubscriber->HandleEvent(event))
				{
					// any results from processing are put onto the secondary queue
					secondary_event_queue_.push(secondary_event);
				}	
			}
		}
		event->processed = true;
	}

	void EventManager::DispatchEventToSubscriber(const shared_ptr<Event>& event, std::string target)
	{
		// Go through each subscriber of the event and have the subscriber handle it
		for (const auto& pSubscriber :  event_subscribers_[event->type])
		{
			if(event_subscribers_.empty()) return; // if reset()
			if(pSubscriber->GetSubscriberName() != target) return;
			
			// Check if the subscriber pointer is valid
			if(pSubscriber)
			{
				// allow subscriber to process the event
				for(const auto &secondary_event : pSubscriber->HandleEvent(event))
				{
					// any results from processing are put onto the secondary queue
					secondary_event_queue_.push(secondary_event);
				}	
			}
		}
		event->processed = true;
	}

	void EventManager::ProcessAllEvents()
	{
		auto event_count = 0;
			
		while(!primary_event_queue_.empty())
		{
			const auto& event = primary_event_queue_.front();

			event->eventId = event_count++;
							
			if(event->processed) // for safety sake
				continue;
			
			// Ask each subscriber to deal with event			
			DispatchEventToSubscriber(event);

			if(!primary_event_queue_.empty())
				primary_event_queue_.pop();
		}

		// Process the secondary queue once primary queue is processed
		while(!secondary_event_queue_.empty())
		{
			// put the secondary queue onto the back of the the primary queue for next cycle of processing
			primary_event_queue_.push(secondary_event_queue_.front());
			secondary_event_queue_.pop();
		}
	}

	std::map<EventType, std::vector<IEventSubscriber*>>& EventManager::GetSubscriptions()
	{
		return event_subscribers_;
	}

	/// <summary>
	/// Handles events that the event manager subscribes to itself
	/// </summary>
	/// <param name="evt"></param>
	/// <returns></returns>
	std::vector<std::shared_ptr<Event>> EventManager::HandleEvent(std::shared_ptr<Event> evt)
	{
		// Does not handle any events in the event queue itself
		std::vector<std::shared_ptr<Event>> generated_events;
		return generated_events;
	}
	std::string EventManager::GetSubscriberName()
	{
		return "EventManager";
	}

	bool IsSameSubscriber(IEventSubscriber* candidate, int subscriptionId) 
	{
		if(candidate)
		{
			auto found_match = candidate == nullptr ? true : candidate->GetSubscriberId() == subscriptionId;
			return found_match;
		}
		return false;
	};
		
	void EventManager::RemoveEventSubscription(const int subscription_id, gamelib::EventType type)
	{
		for(auto &pair : event_subscribers_ )
		{
			auto &type = pair.first;
			auto &subscribers = pair.second;

			// We only remove subscriptions for the specific event type provided
			if(type != pair.first)
			{
				continue;
			}

			// The subscriber's id must match what we have in our subscriptions
			auto matchFn = [&](IEventSubscriber* candidate) { return IsSameSubscriber(candidate, subscription_id); };

			// Look for the subscriber in list of subscribers for this event type
			auto find_result = std::find_if(begin(subscribers), end(subscribers), matchFn);
			
			// Remove if found
			if(find_result != std::end(subscribers))
			{
				subscribers.erase(find_result);
			}
		}		
	}

	void EventManager::Unsubscribe(const int subscription_id)
	{
		for(auto &pair : event_subscribers_ )
		{
			auto &eventSubscribers = pair.second;
			auto matchFn = [&](IEventSubscriber* candidate) { return IsSameSubscriber(candidate, subscription_id); };

			// Look for the subscriber in list of subscribers for this event type
			auto find_result = std::find_if(begin(eventSubscribers), end(eventSubscribers), matchFn);			
			auto found = find_result != std::end(eventSubscribers);

			// Remove if found
			if(found)
			{
				eventSubscribers.erase(find_result);
			}
		}
	}
}

