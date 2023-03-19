#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "ai/BehaviorTree.h"
#include "ai/BehaviorTreeBuilder.h"
#include "ai/InlineActionBehavior.h"
#include "ai/Repeat.h"

using namespace std;


class BehaviorTreeBuilderTests : public testing::Test 
{
 protected:

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};


TEST_F(BehaviorTreeBuilderTests, Behavior_Tree_Builder_Works)
{
	auto dummy1TickCount = 0, dummy2TickCount = 0, dummy3TickCount = 0, dummy4TickCount = 0,
	     dummy5TickCount = 0, dummy6TickCount = 0, dummy7TickCount = 0, dummy8TickCount = 0;

	bool isPlayerVisible = true;
	bool isPlayerInRange = true;
	bool haveWeGotASuspectedLocation = true;
	auto dummyAction5Result = gamelib::BehaviorResult::Success;

	auto* IsPlayerVisible = new gamelib::InlineActionBehavior ([&]{ return isPlayerVisible ? gamelib::BehaviorResult::Success : gamelib::BehaviorResult::Failure;});
	auto* IsPlayerInRange = new gamelib::InlineActionBehavior([&]{ return isPlayerInRange ? gamelib::BehaviorResult::Success : gamelib::BehaviorResult::Failure;});
	auto* HaveWeGotASuspectedLocation = new gamelib::InlineActionBehavior([&] { return haveWeGotASuspectedLocation ? gamelib::BehaviorResult::Success : gamelib::BehaviorResult::Failure; });
	auto* DummyAction1 = new gamelib::InlineActionBehavior([&] { dummy1TickCount++; return gamelib::BehaviorResult::Success; });
	auto* DummyAction2 = new gamelib::InlineActionBehavior([&]{ dummy2TickCount++; return gamelib::BehaviorResult::Success; });
	auto* DummyAction3 = new gamelib::InlineActionBehavior([&] { dummy3TickCount++; return gamelib::BehaviorResult::Success; });
	auto* DummyAction4 = new gamelib::InlineActionBehavior([&]{ dummy4TickCount++; return gamelib::BehaviorResult::Success; });
	auto* DummyAction5 = new gamelib::InlineActionBehavior([&] { dummy5TickCount++; return dummyAction5Result; });
	auto* DummyAction6 = new gamelib::InlineActionBehavior([&] { dummy6TickCount++; return gamelib::BehaviorResult::Success; });
	auto* DummyAction7 = new gamelib::InlineActionBehavior([&]{ dummy7TickCount++; return gamelib::BehaviorResult::Success; });
	auto* DummyAction8 = new gamelib::InlineActionBehavior([&] { dummy8TickCount++; return gamelib::BehaviorResult::Success; });

	gamelib::BehaviorTree* behaviorTree = BehaviorTreeBuilder()
		.ActiveSelector()
			.Sequence()
				.Condition(IsPlayerVisible)
			    .ActiveSelector()
					.Sequence()
						.Condition(IsPlayerInRange)
						.Filter()
							.Action(DummyAction1)
	                        .Finish() 
	                    .Finish()
					.Action(DummyAction2)
	                .Finish()
	            .Finish()
            .Sequence()
				.Condition(HaveWeGotASuspectedLocation)
			    .Action(DummyAction3)
				.Action(DummyAction4)
				.Finish()
			.Sequence()
				.Action(DummyAction5)
				.Action(DummyAction6)
				.Finish()
			.Sequence()
				.Action(DummyAction7)
				.Action(DummyAction8)
				.Finish()
			.Finish()
		.Finish()
	.End();

	behaviorTree->Tick();
	behaviorTree->Tick();
	EXPECT_THAT(dummy1TickCount, testing::Eq(2));
	EXPECT_THAT(dummy2TickCount, testing::Eq(0));
	isPlayerInRange = false;
	behaviorTree->Tick();
	behaviorTree->Tick();
	behaviorTree->Tick();
	EXPECT_THAT(dummy1TickCount, testing::Eq(2));
	EXPECT_THAT(dummy2TickCount, testing::Eq(3));
	isPlayerVisible = false;
	behaviorTree->Tick();
	behaviorTree->Tick();
	EXPECT_THAT(dummy1TickCount, testing::Eq(2));
	EXPECT_THAT(dummy2TickCount, testing::Eq(3));
	EXPECT_THAT(dummy3TickCount, testing::Eq(2));
	EXPECT_THAT(dummy4TickCount, testing::Eq(2));
	haveWeGotASuspectedLocation = false;
	behaviorTree->Tick();
	behaviorTree->Tick();
	behaviorTree->Tick();
	behaviorTree->Tick();
	EXPECT_THAT(dummy1TickCount, testing::Eq(2));
	EXPECT_THAT(dummy2TickCount, testing::Eq(3));
	EXPECT_THAT(dummy3TickCount, testing::Eq(2));
	EXPECT_THAT(dummy4TickCount, testing::Eq(2));
	EXPECT_THAT(dummy5TickCount, testing::Eq(4));
	EXPECT_THAT(dummy6TickCount, testing::Eq(4));

	// Because the previous child is successful, it continues to run because its within a ActiveSelector
	EXPECT_THAT(dummy7TickCount, testing::Eq(0));
	EXPECT_THAT(dummy8TickCount, testing::Eq(0));
	
	// Now make the previous sequence fail and we should run...
	dummyAction5Result = gamelib::BehaviorResult::Failure;
	behaviorTree->Tick();
	behaviorTree->Tick();
	behaviorTree->Tick();
	behaviorTree->Tick();
	EXPECT_THAT(dummy7TickCount, testing::Eq(4));
	EXPECT_THAT(dummy8TickCount, testing::Eq(4));
}