#include "pch.h"
#include "events/UpdateAllGameObjectsEvent.h"
#include "events/EventManager.h"
#include <events/EventSubscriber.h>
using namespace std;
using namespace gamelib;

class DummySubscriber final : public EventSubscriber, public enable_shared_from_this<DummySubscriber>
{
public:
	bool HandleEventReceived = false;
	
	virtual std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs) override
	{
		HandleEventReceived = true;
		return {};
	}
	virtual std::string GetSubscriberName() override
	{
		return "dummy_subscriber";
	}
};

class EventManagerTests : public testing::Test 
{
 protected:
  
  
  DummySubscriber subscriber;	
  const shared_ptr<UpdateAllGameObjectsEvent> the_event = std::make_shared<UpdateAllGameObjectsEvent>();
  const EventId Id = UpdateAllGameObjectsEventTypeEventId;

  void SetUp() override
  {
	 EventManager::Get()->ClearSubscribers();
  	 EventManager::Get()->Initialize();
  }

  void TearDown() override 
  {
  }
  
  static shared_ptr<EventManager> create_event_manager()
  {
  	return shared_ptr<EventManager>(EventManager::Get());
  }
	
};

TEST_F(EventManagerTests, InitializeTest)
{
	EXPECT_TRUE(EventManager::Get()->Initialize()) << "Expected the initialization of event manager to succeed";
}

TEST_F(EventManagerTests, RaiseEvent)
{	
	EventManager::Get()->RaiseEvent(dynamic_pointer_cast<Event>(the_event), &subscriber);
	EXPECT_EQ(1, EventManager::Get()->CountReady()) << "Expected 1 event to be ready for processing";
}

TEST_F(EventManagerTests, ProcessEvent)
{		
	EventManager::Get()->RaiseEvent(dynamic_pointer_cast<Event>(the_event), &subscriber);
	EventManager::Get()->ProcessAllEvents();

	EXPECT_EQ(0, EventManager::Get()->CountReady());
	EXPECT_TRUE(the_event->Processed) << "The event was not marked as processed";
}

TEST_F(EventManagerTests, SubscribeToEvent)
{
	// When subscribing to an event, a processed event should notify the subscriber of that event
	EventManager::Get()->SubscribeToEvent(Id, &subscriber);
	EventManager::Get()->RaiseEvent(dynamic_pointer_cast<Event>(the_event), &subscriber);
	EventManager::Get()->ProcessAllEvents();

	EXPECT_EQ(0, EventManager::Get()->CountReady()) << "Expected there to be no events waiting to be dispatched after processing all events";
	EXPECT_TRUE(subscriber.HandleEventReceived) << "Subscriber was not notified after processing events";

	auto& all_event_subscribers = EventManager::Get()->GetSubscriptions()[Id];
	
	EXPECT_EQ(1, all_event_subscribers.size()) << "Expect 1 subscriber to exist";
	//EXPECT_STREQ(all_event_subscribers[0].lock()->GetSubscriberName().c_str(), subscriber->GetSubscriberName().c_str()) << "Our subscriber was not the subscription w eexepcted";
}

TEST_F(EventManagerTests, DispatchEventDirectlyToSubscriber)
{
	EventManager::Get()->SubscribeToEvent(Id, &subscriber);
	EventManager::Get()->DispatchEventToSubscriber(the_event, 0UL);
	EXPECT_EQ(EventManager::Get()->CountReady(), 0) << "Direct dispatch of event to subscriber should not show up in event queues";
	EXPECT_TRUE(subscriber.HandleEventReceived) << "subscriber was not directly notified";
	EXPECT_TRUE(the_event->Processed) << "The event was not marked as processed";
}

TEST_F(EventManagerTests, ClearSubscribersTest)
{
	EventManager::Get()->SubscribeToEvent(Id, &subscriber);
	EventManager::Get()->RaiseEvent(dynamic_pointer_cast<Event>(the_event), &subscriber);
	EventManager::Get()->ProcessAllEvents();

	auto& all_event_subscribers = EventManager::Get()->GetSubscriptions();

	EXPECT_EQ(1, all_event_subscribers.size()) << "Expect more than 0 subscriber to exist";

	//Clear subscriptions
	EventManager::Get()->ClearSubscribers();

	EXPECT_EQ(0, all_event_subscribers.size()) << "Expected all the subscribers to have been removed";

}

TEST_F(EventManagerTests, CantSubscribeAgain)
{
	EventManager::Get()->SubscribeToEvent(Id, &subscriber);
	EventManager::Get()->SubscribeToEvent(Id, &subscriber);

	auto& all_event_subscribers = EventManager::Get()->GetSubscriptions();

	EXPECT_TRUE(all_event_subscribers[Id].size(), 1);

}