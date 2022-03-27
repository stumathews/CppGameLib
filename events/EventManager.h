#pragma once
#include <vector>
#include <memory>
#include "EventSubscriber.h"
#include <map>
#include <queue>
#include "Event.h"
#include "common/Logger.h"
#include "util/SettingsManager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>

namespace gamelib
{
	class EventManager : public EventSubscriber
	{

	public:
		static EventManager* Get();
		size_t count_ready() const;

		// Clears subscribers, primary and secondary queues
		void reset();

		// Cannot copy an event manager
		EventManager(EventManager const&) = delete;
		~EventManager();
		
		// Cannot assign to an event manager
		void operator=(EventManager const&) = delete;

		bool Initialize();

		void ClearSubscribers();
		
		// Raise an arbitrary event
		void RaiseEvent(const std::shared_ptr<Event> event, IEventSubscriber* you);

		// Add yourself as an event handler (aka event subscriber) to handle an event of type EventType
		void SubscribeToEvent(EventType type, IEventSubscriber* pYou);

		// Directly send event to subscriber without going through the event queue
		void DispatchEventToSubscriber(const std::shared_ptr<Event>& event);
		
		// Goes through all the events on the primary and secondary queues and passes them to their respective event handlers
		// Primary queue is incremented with event when an event is raised via the RaiseEvent() function. This can be one at any time.
		// secondary queue is composed automatically of events that occured while processing the primary queue. 
		void ProcessAllEvents();

		std::map<EventType, std::vector<IEventSubscriber*>>& GetSubscriptions();


		// Inherited via IEventSubscriber
		
		virtual std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt) override;

		virtual std::string GetSubscriberName() override;

		void RemoveEventSubscription(const int subscription_id, gamelib::EventType type);
		void Unsubscribe(const int subscription_id);
	protected:
		static EventManager* Instance;
	private:
		EventManager();

		// Primary queue used for event processing
		std::queue<std::shared_ptr<Event>> primary_event_queue_;

		// used to hold events occurring out of processing of primary events
		std::queue<std::shared_ptr<Event>> secondary_event_queue_;

		// Event subscribers (aka event handlers)
		std::map<EventType, std::vector<IEventSubscriber*>> event_subscribers_;
		bool resetting = false;
	};
}

