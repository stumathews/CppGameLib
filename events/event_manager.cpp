#include "event_manager.h"
#include <vector>
#include <algorithm>
#include "Event.h"
#include "event_subscriber.h"
#include "common/Common.h"
#include "util/RectDebugging.h"

using namespace std;
namespace gamelib
{

	event_manager::event_manager(shared_ptr<settings_manager> config, std::shared_ptr<logger> the_logger): the_logger(std::move(the_logger)) {
		this->config = config;
	}

	size_t event_manager::count_ready()
	{
		return primary_event_queue_.size() + secondary_event_queue_.size();
	}

	bool event_manager::initialize()
	{
		
		return run_and_log("event_manager::initialize()", config->get_bool("global", "verbose"), [&]()
		{
			return true;
		}, true, true, config);
	}

	void event_manager::raise_event(const shared_ptr<event> event, event_subscriber* you)  // NOLINT(performance-unnecessary-value-param)
	{
		auto const log = "event_manager: " + you->get_subscriber_name()  + string(" raised to event ") + event->to_str();
		
		if(event->type != event_type::DoLogicUpdateEventType)
			the_logger->log_message(log);

		primary_event_queue_.push(event);
	}

	void event_manager::subscribe_to_event(const event_type type, event_subscriber* you)
	{
		auto const message = "event_manager: "+you->get_subscriber_name() + string(" subscribed to event ") + type;
		the_logger->log_message(message);
		
		event_subscribers_[type].push_back(you);	
	}

	void event_manager::dispatch_event_to_subscriber(const shared_ptr<event>& event)
	{
		for (const auto& subscriber :  event_subscribers_[event->type])
		{
			// allow subscriber to process the event
			for(const auto &secondary_event : subscriber->handle_event(event))
			{
				// any results from processing are put onto the secondary queue
				secondary_event_queue_.push(secondary_event);
			}			
		}
		event->processed = true;
	}

	void event_manager::process_all_events()
	{
		auto event_count = 0;
			
		while(!primary_event_queue_.empty())
		{
			const auto& event = primary_event_queue_.front();

			event->event_id = event_count++;
							
			if(event->processed) // for safety sake
				continue;
			
			// get subscribers of this event type
			
			dispatch_event_to_subscriber(event);
			
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

	std::map<event_type, std::vector<event_subscriber*>> event_manager::get_subscriptions() const
	{
		return event_subscribers_;
	}
}

