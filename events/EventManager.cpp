#include "EventManager.h"
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

	EventManager::EventManager(SettingsManager& config, logger& the_logger) : gameLogger(the_logger), config(config) { 	}

	size_t EventManager::count_ready() const
	{
		return primary_event_queue_.size() + secondary_event_queue_.size();
	}

	void EventManager::reset()
	{
		std::queue<shared_ptr<event>> empty;
		  std::swap( primary_event_queue_, empty );
		  std::swap( secondary_event_queue_, empty );
		//this->event_subscribers_.clear();
		  
	}

	EventManager::~EventManager()
	{
		gameLogger.log_message("Event manager dying.");
	}

	bool EventManager::initialize()
	{
		
		return run_and_log("EventManager::initialize()", config.get_bool("global", "verbose"), [&]()
		{
			subscribe_to_event(gamelib::event_type::GameObject, this);
			return true;
		}, config, true, true);
	}

	void EventManager::raise_event(const shared_ptr<event> event, IEventSubscriber* you)  // NOLINT(performance-unnecessary-value-param)
	{		
		if(!you)
		{
			gameLogger.log_message("Invalid sender", true);
			return;
		}

		auto const log = "EventManager: " + you->get_subscriber_name()  + string(" raised to event ") + event->to_str();
		
		if(event->type != event_type::DoLogicUpdateEventType)
			gameLogger.log_message(log);

		primary_event_queue_.push(event);		
	}

	void EventManager::subscribe_to_event(const event_type type, IEventSubscriber* you)
	{
		if(you)
		{
			auto const message = "EventManager: "+you->get_subscriber_name() + string(" subscribed to event ") + type;
			gameLogger.log_message(message);
		
			event_subscribers_[type].push_back(you);
		}
	}

	void EventManager::dispatch_event_to_subscriber(const shared_ptr<event>& event)
	{
		// Go through each subscriber of the event and have the subscriber handle it
		for (const auto& pSubscriber :  event_subscribers_[event->type])
		{
			if(event_subscribers_.empty()) return; // if reset()
			
			// Check if the subscriber pointer is valid
			if(pSubscriber)
			{
				// allow subscriber to process the event
				for(const auto &secondary_event : pSubscriber->handle_event(event))
				{
					// any results from processing are put onto the secondary queue
					secondary_event_queue_.push(secondary_event);
				}	
			}
		}
		event->processed = true;
	}

	void EventManager::process_all_events()
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

	std::map<event_type, std::vector<IEventSubscriber*>> EventManager::get_subscriptions() const
	{
		return event_subscribers_;
	}
	std::vector<std::shared_ptr<event>> EventManager::handle_event(std::shared_ptr<event> evt)
	{
		std::vector<std::shared_ptr<event>> generated_events;

		if(evt->type == gamelib::event_type::GameObject)
		{
			const auto game_object_event = dynamic_pointer_cast<gamelib::GameObjectEvent>(evt);
			//remove_subscription(game_object_event->game_object->id, gamelib::event_type::PositionChangeEventType);
		}

		return generated_events;
	}
	std::string EventManager::get_subscriber_name()
	{
		return "EventManager";
	}

	bool IsSameSubscriber(IEventSubscriber* candidate, int subscriptionId) 
	{
		if(candidate)
		{
			auto found_match = candidate == nullptr ? true : candidate->get_subscriber_id() == subscriptionId;
			return found_match;
		}
		return false;
	};

	void EventManager::remove_subscription(const int subscription_id, gamelib::event_type type)
	{
		for(auto &pair : event_subscribers_ )
		{
			auto &type = pair.first;
			auto &subscribers = pair.second;

			// We only remove subscriptions for the specific event type provided
			if(type != pair.first)
				continue;

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

	// ubsubscribe to all events
	void EventManager::Unsubscribe(const int subscription_id)
	{
		for(auto &pair : event_subscribers_ )
		{
			auto &eventSubscribers = pair.second;
			auto matchFn = [&](IEventSubscriber* candidate) { return IsSameSubscriber(candidate, subscription_id); };

			// Look for the subscriber in list of subscribers for this event type
			auto find_result = std::find_if(begin(eventSubscribers), end(eventSubscribers), matchFn);
			
			// Remove if found
			if(find_result != std::end(eventSubscribers))
			{
				eventSubscribers.erase(find_result);
			}
		}
	}
}

