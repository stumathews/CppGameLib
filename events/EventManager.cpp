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

using namespace std;
namespace gamelib
{	
	EventManager::EventManager() : logEvents(false) { 	}

	std::map<EventType, std::vector<IEventSubscriber*>>& EventManager::GetSubscriptions() { return event_subscribers_; }
	std::string EventManager::GetSubscriberName() { return "EventManager"; }
	void EventManager::ClearSubscribers() { this->event_subscribers_.clear(); }

	EventManager* EventManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new EventManager();
		}
		return Instance;
	}

	EventManager* EventManager::Instance = nullptr;

	size_t EventManager::count_ready() const { return primary_event_queue_.size() + secondary_event_queue_.size(); }

	void EventManager::Reset()
	{
		std::queue<shared_ptr<Event>> empty;
		std::swap( primary_event_queue_, empty );
		std::swap( secondary_event_queue_, empty );		  
	}

	EventManager::~EventManager() { Logger::Get()->LogThis("Event manager dying."); Instance = nullptr; }

	
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
		primary_event_queue_.push(event);		

		LogEventRaised(you, event);
	}

	void EventManager::LogEventRaised(IEventSubscriber* you, const std::shared_ptr<Event>& event) const
	{
		if (logEvents)
		{
			std::stringstream log;
			log << "EventManager: " << you->GetSubscriberName() << " raised to event " << ToString(event->Type);

			if (event->Type != EventType::UpdateAllGameObjectsEventType) { Logger::Get()->LogThis(log.str()); }
		}
	}

	void EventManager::RaiseEventWithNoLogging(const std::shared_ptr<Event>& event) { primary_event_queue_.push(event); }
		
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
	void EventManager::DispatchEventToSubscriber(const shared_ptr<Event>& event, const unsigned long deltaMs)
	{
		// Go through each subscriber of the event and have the subscriber handle it
		for (const auto& pSubscriber :  event_subscribers_[event->Type])
		{
			if (event_subscribers_.empty()) { return; } 
			if (!pSubscriber) { continue; }
			
			// allow subscriber to process the event
			for(const auto &secondary_event : pSubscriber->HandleEvent(event, deltaMs))
			{
				AddToSecondaryEventQueue(secondary_event, pSubscriber);
			}	
			
		}
		event->Processed = true;
	}

	void EventManager::AddToSecondaryEventQueue(const std::shared_ptr<Event>& secondary_event, IEventSubscriber* originSubscriber)
	{
		// any results from processing are put onto the secondary queue
		secondary_event->Origin = originSubscriber->GetSubscriberName();
		secondary_event_queue_.push(secondary_event);
	}

	void EventManager::DispatchEventToSubscriber(const shared_ptr<Event>& event, const std::string& target)
	{
		// Go through each subscriber of the event and have the subscriber handle it
		for (const auto& pSubscriber :  event_subscribers_[event->Type])
		{
			if (event_subscribers_.empty()) { return; } // if reset()
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
		while(!primary_event_queue_.empty())
		{
			const auto& event = primary_event_queue_.front();
							
			if (event->Processed) { continue; }
			
			// Ask each subscriber to deal with event			
			DispatchEventToSubscriber(event, deltaMs);

			if (!primary_event_queue_.empty()) { primary_event_queue_.pop(); }
		}

		// Process the secondary queue once primary queue is processed
		while(!secondary_event_queue_.empty())
		{
			// put the secondary queue onto the back of the the primary queue for next cycle of processing
			primary_event_queue_.push(secondary_event_queue_.front());
			secondary_event_queue_.pop();
		}
	}



	/// <summary>
	/// Handles events that the event manager subscribes to itself
	/// </summary>
	/// <param name="evt"></param>
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
		
	void EventManager::RemoveEventSubscription(const int subscriptionId, EventType type)
	{
		for(auto &pair : event_subscribers_ )
		{
			auto &eventType = pair.first;
			auto &subscribers = pair.second;

			// We only remove subscriptions for the specific event type provided
			if(eventType != pair.first) { continue; }

			// The subscriber's id must match what we have in our subscriptions
			auto matchFn = [&](IEventSubscriber* candidate) { return IsSameSubscriber(candidate, subscriptionId); };

			// Look for the subscriber in list of subscribers for this event type
			auto find_result = std::find_if(begin(subscribers), end(subscribers), matchFn);
			
			// Remove if found
			if(find_result != std::end(subscribers)) { subscribers.erase(find_result); }
		}		
	}

	void EventManager::Unsubscribe(const int subscriptionId)
	{
		for(auto &pair : event_subscribers_ )
		{
			auto &eventSubscribers = pair.second;
			auto matchFn = [&](IEventSubscriber* candidate) { return IsSameSubscriber(candidate, subscriptionId); };

			// Look for the subscriber in list of subscribers for this event type
			auto findResult = std::find_if(begin(eventSubscribers), end(eventSubscribers), matchFn);

			if(findResult != std::end(eventSubscribers)) {	eventSubscribers.erase(findResult); }
		}
	}
}

