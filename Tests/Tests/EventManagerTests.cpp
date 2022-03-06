#include "pch.h"
#include "common/static_config.h"
#include "events/DoLogicUpdateEvent.h"
#include "events/EventManager.h"
#include <events/EventSubscriber.h>
#include <objects/MultipleInheritableEnableSharedFromThis.h>

using namespace std;
using namespace gamelib;

class dummy_subscriber final : public EventSubscriber, public inheritable_enable_shared_from_this<dummy_subscriber>
{
public:
	bool handle_event_received = false;
	
	virtual std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> evt) override
	{
		handle_event_received = true;
		return std::vector<std::shared_ptr<event>>();
	}
	virtual std::string get_subscriber_name() override
	{
		return "dummy_subscriber";
	}
};

class EventManagerTests : public ::testing::Test {
 protected:
  
  shared_ptr<EventManager> event_admin;
  SettingsManager settings;
  dummy_subscriber subscriber;	
  logger logger;
  do_logic_update_event logic_update_event;
  const shared_ptr<do_logic_update_event> the_event = shared_ptr<do_logic_update_event>(new do_logic_update_event());
  const event_type event_type = event_type::DoLogicUpdateEventType;

  void SetUp() override
  {
	 event_admin = create_event_manager(settings, logger);
  	 event_admin->initialize();
  }

  void TearDown() override {}
  static shared_ptr<static_config> create_config() {	
  	return shared_ptr<static_config>(new static_config());
  }

  static shared_ptr<EventManager> create_event_manager(SettingsManager& config, gamelib::logger& logger) {
  	return shared_ptr<EventManager>(new EventManager(config, logger));
  }
	
};

TEST_F(EventManagerTests, InitializeTest)
{
	EXPECT_TRUE(event_admin->initialize()) << "Expected the initialization of event manager to succeed";
}

TEST_F(EventManagerTests, RaiseEvent)
{	
	event_admin->raise_event(dynamic_pointer_cast<event>(the_event), &subscriber);
	EXPECT_EQ(1, event_admin->count_ready()) << "Expected 1 event to be ready for processing";
}

TEST_F(EventManagerTests, ProcessEvent)
{		
	event_admin->raise_event(dynamic_pointer_cast<event>(the_event), &subscriber);
	event_admin->process_all_events();

	EXPECT_EQ(0, event_admin->count_ready());
	EXPECT_TRUE(the_event->processed) << "The event was not marked as processed";
}

TEST_F(EventManagerTests, SubscribeToEvent)
{
	// When subscribing to an event, a processed event should notify the subscriber of that event
	event_admin->subscribe_to_event(event_type, &subscriber);
	event_admin->raise_event(dynamic_pointer_cast<event>(the_event), &subscriber);
	event_admin->process_all_events();

	EXPECT_EQ(0, event_admin->count_ready()) << "Expected there to be no events waiting to be dispatched after processing all events";
	EXPECT_TRUE(subscriber.handle_event_received) << "Subscriber was not notified after processing events";

	auto &all_event_subscribers = event_admin->get_subscriptions()[event_type];
	
	EXPECT_EQ(1, all_event_subscribers.size()) << "Expect 1 subscriber to exist";
	//EXPECT_STREQ(all_event_subscribers[0].lock()->get_subscriber_name().c_str(), subscriber->get_subscriber_name().c_str()) << "Our subscriber was not the subscription w eexepcted";
}

TEST_F(EventManagerTests, DispatchEventDirectlyToSubscriber)
{
	event_admin->subscribe_to_event(event_type, &subscriber);
	event_admin->dispatch_event_to_subscriber(the_event);
	EXPECT_EQ(event_admin->count_ready(), 0) << "Direct dispatch of event to subscriber should not show up in event queues";
	EXPECT_TRUE(subscriber.handle_event_received) << "subscriber was not directly notified";
	EXPECT_TRUE(the_event->processed) << "The event was not marked as processed";
}