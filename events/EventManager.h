#pragma once
#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <vector>
#include <memory>
#include "EventSubscriber.h"
#include <map>
#include <queue>
#include "Event.h"
#include <functional>

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
		
		void RemoveEventSubscription(int subscriptionId, const EventId& id);
		void Unsubscribe(int subscriptionId);
		void Reset(); // Clears subscribers, primary and secondary queues		
		void ClearSubscribers();
		void RaiseEvent(const std::shared_ptr<Event>& event, IEventSubscriber* you);
		void RaiseEventWithNoLogging(const std::shared_ptr<Event>& event);
		void LogEventSubscription(const EventId& eventId, IEventSubscriber* pYou) const;
		void SubscribeToEvent(const EventId& eventId, IEventSubscriber* pYou);
		void DispatchEventToSubscriber(const std::shared_ptr<Event>& event, unsigned long deltaMs);
		void DispatchEventToSubscriber(const std::shared_ptr<Event>& event, const std::string& target);

		// Goes through all the events on the primary and secondary queues and passes them to their respective event handlers
		// Primary queue is incremented with event when an event is raised via the RaiseEvent() function. This can be one at any time.
		// secondary queue is composed automatically of events that happened while processing the primary queue. 
		void ProcessAllEvents(unsigned long deltaMs = 0UL);
				
		bool Initialize();
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs) override;
		std::string GetSubscriberName() override;
		std::map<const EventId, std::vector<IEventSubscriber*>>& GetSubscriptions();
		[[nodiscard]] size_t CountReady() const;
		std::queue<std::shared_ptr<Event>> GetEvents();
		void SetEventTap(const std::function<void(const std::shared_ptr<Event>& event, const IEventSubscriber* pSubscriber)>& tapFn);
	protected:
		static EventManager* instance;
	private:
		EventManager();		
		void Send(const std::shared_ptr<Event>& event, IEventSubscriber* pSubscriber, const unsigned long deltaMs = 0);		
		void AddToSecondaryEventQueue(const std::shared_ptr<Event>& secondaryEvent, IEventSubscriber* originSubscriber);
		void LogEventRaised(IEventSubscriber* you, const std::shared_ptr<Event>& event) const;
		std::queue<std::shared_ptr<Event>> primaryEventQueue; // Primary queue used for event processing 		
		std::queue<std::shared_ptr<Event>> secondaryEventQueue; // used to hold events occurring out of processing of primary events
		std::map<const EventId, std::vector<IEventSubscriber*>> eventSubscribers;
		std::map<EventId, int> eventsDispatched {};
		int badSubscribersDuringDispatch {};
		int noSubscribersDuringDispatch {};
		int dispatchCalledTimes {};
		unsigned long elapsedTimeMs {};
		bool logEvents;
		bool printStatistics;
		std::function<void(const std::shared_ptr<Event>& event, IEventSubscriber* pSubscriber)> tap = nullptr;
	};
}

#endif

