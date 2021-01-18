#include "event_manager.h"
#include <vector>
#include <algorithm>
#include "Event.h"
#include "IEventSubscriber.h"
#include "common/Common.h"
#include "util/RectDebugging.h"
#include <events/GameObjectEvent.h>

using namespace std;
namespace gamelib
{

	event_manager::event_manager(shared_ptr<settings_manager> config, std::shared_ptr<logger> the_logger): the_logger(std::move(the_logger)) {
		this->config = config;
	}

	size_t event_manager::count_ready() const
	{
		return primary_event_queue_.size() + secondary_event_queue_.size();
	}

	void event_manager::reset()
	{
		std::queue<shared_ptr<event>> empty;
		  std::swap( primary_event_queue_, empty );
		  std::swap( secondary_event_queue_, empty );
		//this->event_subscribers_.clear();
		  
	}

	bool event_manager::initialize()
	{
		
		return run_and_log("event_manager::initialize()", config->get_bool("global", "verbose"), [&]()
		{
				subscribe_to_event(gamelib::event_type::GameObject, shared_from_this());
			return true;
		}, true, true, config);
	}

	void event_manager::raise_event(const shared_ptr<event> event, shared_ptr<IEventSubscriber> you)  // NOLINT(performance-unnecessary-value-param)
	{		
		auto const log = "event_manager: " + you->get_subscriber_name()  + string(" raised to event ") + event->to_str();
		
		if(event->type != event_type::DoLogicUpdateEventType)
			the_logger->log_message(log);

		primary_event_queue_.push(event);		
	}

	void event_manager::subscribe_to_event(const event_type type, weak_ptr<IEventSubscriber> pYou)
	{
		if(auto you = pYou.lock())
		{
			auto const message = "event_manager: "+you->get_subscriber_name() + string(" subscribed to event ") + type;
			the_logger->log_message(message);
		
			event_subscribers_[type].push_back(you);
		}
	}

	void event_manager::dispatch_event_to_subscriber(const shared_ptr<event>& event)
	{
		for (const auto& pSubscriber :  event_subscribers_[event->type])
		{
			if(event_subscribers_.empty()) return; // if reset()
			if(auto subscriber = pSubscriber.lock())
			{
				// allow subscriber to process the event
				for(const auto &secondary_event : subscriber->handle_event(event))
				{
					// any results from processing are put onto the secondary queue
					secondary_event_queue_.push(secondary_event);
				}	
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

	std::map<event_type, std::vector<weak_ptr<IEventSubscriber>>> event_manager::get_subscriptions() const
	{
		return event_subscribers_;
	}
	std::vector<std::shared_ptr<event>> event_manager::handle_event(std::shared_ptr<event> evt)
	{
		std::vector<std::shared_ptr<event>> generated_events;

		if(evt->type == gamelib::event_type::GameObject)
		{
			const auto game_object_event = dynamic_pointer_cast<gamelib::GameObjectEvent>(evt);
			//remove_subscription(game_object_event->game_object->id, gamelib::event_type::PositionChangeEventType);
		}

		return generated_events;
	}
	std::string event_manager::get_subscriber_name()
	{
		return "event_manager";
	}
	void event_manager::remove_subscription(const int subscription_id, gamelib::event_type type)
	{
		for(auto &pair : event_subscribers_ )
		{
			auto &type = pair.first;
			auto &subscribers = pair.second;
			if(type != pair.first)
				continue;
			std::remove_if(begin(subscribers), end(subscribers), [&](weak_ptr<IEventSubscriber> pCandidate) 
				{
					if(auto candidate = pCandidate.lock())
					{
						return candidate == nullptr 
							? false 
							: candidate->get_subscriber_id() == subscription_id; 
					}
				});
		}
		
		int i = 0;
	}
}

