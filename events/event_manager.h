#pragma once
#include <vector>
#include <memory>
#include "event_subscriber.h"
#include <map>
#include <queue>
#include "Event.h"
#include "common/static_config.h"
#include "common/Logger.h"
#include "util/settings_manager.h"

namespace gamelib
{
	class event_manager
	{
		// Primary queue used for event processing
		std::queue<std::shared_ptr<event>> primary_event_queue_;
		
		// used to hold events occurring out of processing of primary events
		std::queue<std::shared_ptr<event>> secondary_event_queue_;

		// Event subscribers (aka event handlers)
		std::map<event_type, std::vector<event_subscriber*>> event_subscribers_;

		std::shared_ptr<settings_manager> config;
		std::shared_ptr<logger> the_logger;

	public:
		event_manager(std::shared_ptr<settings_manager> config, std::shared_ptr<logger> the_logger = std::make_shared<logger>());
		~event_manager() = default;
		size_t count_ready();

		// Cannot copy an event manager
		event_manager(event_manager const&) = delete;
		
		// Cannot assign to an event manager
		void operator=(event_manager const&) = delete;

		bool initialize();
		
		// Raise an arbitrary event
		void raise_event(const std::shared_ptr<event> event, event_subscriber* you);

		// Add yourself as an event handler (aka event subscriber) to handle an event of type event_type
		void subscribe_to_event(event_type type, event_subscriber* you);

		// Directly send event to subscriber without going through the event queue
		void dispatch_event_to_subscriber(const std::shared_ptr<event>& event);
		
		// Goes through all the events on the primary and secondary queues and passes them to their respective event handlers
		// Primary queue is incremented with event when an event is raised via the raise_event() function. This can be one at any time.
		// secondary queue is composed automatically of events that occured while processing the primary queue. 
		void process_all_events();

		[[nodiscard]] std::map<event_type, std::vector<event_subscriber*>> get_subscriptions() const;

	};
}

