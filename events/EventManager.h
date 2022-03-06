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
	class EventManager : public EventSubscriber, public inheritable_enable_shared_from_this<IEventSubscriber>
	{
		// Primary queue used for event processing
		std::queue<std::shared_ptr<event>> primary_event_queue_;
		
		// used to hold events occurring out of processing of primary events
		std::queue<std::shared_ptr<event>> secondary_event_queue_;

		// Event subscribers (aka event handlers)
		std::map<event_type, std::vector<IEventSubscriber*>> event_subscribers_;

		SettingsManager& config;
		Logger& gameLogger;
		bool resetting = false;

	public:
		EventManager(SettingsManager& config, Logger& Logger);
		size_t count_ready() const;

		// Clears subscribers, primary and secondary queues
		void reset();

		// Cannot copy an event manager
		EventManager(EventManager const&) = delete;
		~EventManager();
		
		// Cannot assign to an event manager
		void operator=(EventManager const&) = delete;

		bool initialize();
		
		// Raise an arbitrary event
		void raise_event(const std::shared_ptr<event> event, IEventSubscriber* you);

		// Add yourself as an event handler (aka event subscriber) to handle an event of type event_type
		void subscribe_to_event(event_type type, IEventSubscriber* pYou);

		// Directly send event to subscriber without going through the event queue
		void dispatch_event_to_subscriber(const std::shared_ptr<event>& event);
		
		// Goes through all the events on the primary and secondary queues and passes them to their respective event handlers
		// Primary queue is incremented with event when an event is raised via the raise_event() function. This can be one at any time.
		// secondary queue is composed automatically of events that occured while processing the primary queue. 
		void process_all_events();

		[[nodiscard]] std::map<event_type, std::vector<IEventSubscriber*>> get_subscriptions() const;


		// Inherited via IEventSubscriber
		
		virtual std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> evt) override;

		virtual std::string get_subscriber_name() override;

		void remove_subscription(const int subscription_id, gamelib::event_type type);
		void Unsubscribe(const int subscription_id);

	};
}

