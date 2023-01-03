#pragma once
#include <vector>
#include <memory>
#include "EventSubscriber.h"
#include <map>
#include <queue>
#include "Event.h"
#include "common/Logger.h"

namespace gamelib
{
	
	// Raises and dispatches events to subscribers	
	class EventManager : public EventSubscriber
	{

	public:
		static EventManager* Get();
		EventManager(EventManager const&) = delete; // Cannot copy an event manager
		~EventManager() override;
		void operator=(EventManager const&) = delete; // Cannot assign to an event manager
		
		void RemoveEventSubscription(int subscriptionId, EventType type);
		void Unsubscribe(int subscriptionId);
		void Reset(); // Clears subscribers, primary and secondary queues		
		void ClearSubscribers();
		void RaiseEvent(std::shared_ptr<Event> event, IEventSubscriber* you);
		void RaiseEventWithNoLogging(const std::shared_ptr<Event>& event);
		void SubscribeToEvent(EventType type, IEventSubscriber* pYou);
		void DispatchEventToSubscriber(const std::shared_ptr<Event>& event, unsigned long deltaMs);
		void DispatchEventToSubscriber(const std::shared_ptr<Event>& event, const std::string& target);
		// Goes through all the events on the primary and secondary queues and passes them to their respective event handlers
		// Primary queue is incremented with event when an event is raised via the RaiseEvent() function. This can be one at any time.
		// secondary queue is composed automatically of events that occured while processing the primary queue. 
		void ProcessAllEvents(unsigned long deltaMs = 0UL);
				
		bool Initialize();
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt, unsigned long deltaMs) override;
		std::string GetSubscriberName() override;
		std::map<EventType, std::vector<IEventSubscriber*>>& GetSubscriptions();
		[[nodiscard]] size_t count_ready() const;
	protected:
		static EventManager* Instance;
	private:
		EventManager();	
		void AddToSecondaryEventQueue(const std::shared_ptr<Event>& event, IEventSubscriber* originator);
		void LogEventRaised(IEventSubscriber* you, const std::shared_ptr<Event>& event) const;
		std::queue<std::shared_ptr<Event>> primary_event_queue_; // Primary queue used for event processing 		
		std::queue<std::shared_ptr<Event>> secondary_event_queue_; // used to hold events occurring out of processing of primary events
		std::map<EventType, std::vector<IEventSubscriber*>> event_subscribers_;
		bool logEvents;
	};
}

