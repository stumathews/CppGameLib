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
	
	virtual std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt) override
	{
		handle_event_received = true;
		return std::vector<std::shared_ptr<Event>>();
	}
	virtual std::string GetSubscriberName() override
	{
		return "dummy_subscriber";
	}
};

class EventManagerTests : public ::testing::Test {
 protected:
  
  shared_ptr<EventManager> event_admin;
  SettingsManager settings;
  dummy_subscriber subscriber;	
  Logger Logger;
  do_logic_update_event logic_update_event;
  const shared_ptr<do_logic_update_event> the_event = shared_ptr<do_logic_update_event>(new do_logic_update_event());
  const EventType EventType = EventType::DoLogicUpdateEventType;

  void SetUp() override
  {
	 event_admin = create_event_manager(settings, Logger);
  	 event_admin->Initialize();
  }

  void TearDown() override {}
  static shared_ptr<static_config> create_config() {	
  	return shared_ptr<static_config>(new static_config());
  }

  static shared_ptr<EventManager> create_event_manager(SettingsManager& config, gamelib::Logger& Logger) {
  	return shared_ptr<EventManager>(new EventManager(config, Logger));
  }
	
};

TEST_F(EventManagerTests, InitializeTest)
{
	EXPECT_TRUE(event_admin->Initialize()) << "Expected the initialization of event manager to succeed";
}

TEST_F(EventManagerTests, RaiseEvent)
{	
	event_admin->RaiseEvent(dynamic_pointer_cast<Event>(the_event), &subscriber);
	EXPECT_EQ(1, event_admin->count_ready()) << "Expected 1 event to be ready for processing";
}

TEST_F(EventManagerTests, ProcessEvent)
{		
	event_admin->RaiseEvent(dynamic_pointer_cast<Event>(the_event), &subscriber);
	event_admin->ProcessAllEvents();

	EXPECT_EQ(0, event_admin->count_ready());
	EXPECT_TRUE(the_event->processed) << "The event was not marked as processed";
}

TEST_F(EventManagerTests, SubscribeToEvent)
{
	// When subscribing to an event, a processed event should notify the subscriber of that event
	event_admin->SubscribeToEvent(EventType, &subscriber);
	event_admin->RaiseEvent(dynamic_pointer_cast<Event>(the_event), &subscriber);
	event_admin->ProcessAllEvents();

	EXPECT_EQ(0, event_admin->count_ready()) << "Expected there to be no events waiting to be dispatched after processing all events";
	EXPECT_TRUE(subscriber.handle_event_received) << "Subscriber was not notified after processing events";

	auto &all_event_subscribers = event_admin->GetSubscriptions()[EventType];
	
	EXPECT_EQ(1, all_event_subscribers.size()) << "Expect 1 subscriber to exist";
	//EXPECT_STREQ(all_event_subscribers[0].lock()->GetSubscriberName().c_str(), subscriber->GetSubscriberName().c_str()) << "Our subscriber was not the subscription w eexepcted";
}

TEST_F(EventManagerTests, DispatchEventDirectlyToSubscriber)
{
	event_admin->SubscribeToEvent(EventType, &subscriber);
	event_admin->DispatchEventToSubscriber(the_event);
	EXPECT_EQ(event_admin->count_ready(), 0) << "Direct dispatch of event to subscriber should not show up in event queues";
	EXPECT_TRUE(subscriber.handle_event_received) << "subscriber was not directly notified";
	EXPECT_TRUE(the_event->processed) << "The event was not marked as processed";
}