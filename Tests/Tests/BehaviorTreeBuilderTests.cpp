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
	auto* FireAtPlayer = new gamelib::InlineActionBehavior([&] { dummy1TickCount++; return gamelib::BehaviorResult::Success; });
	auto* MoveTowardsPlayer = new gamelib::InlineActionBehavior([&]{ dummy2TickCount++; return gamelib::BehaviorResult::Success; });
	auto* MovePlayerToLastKnownLocation = new gamelib::InlineActionBehavior([&] { dummy3TickCount++; return gamelib::BehaviorResult::Success; });
	auto* LookAround = new gamelib::InlineActionBehavior([&]{ dummy4TickCount++; return gamelib::BehaviorResult::Success; });
	auto* MoveToRandomPosition = new gamelib::InlineActionBehavior([&] { dummy5TickCount++; return dummyAction5Result; });
	auto* LookAroundSomeMore = new gamelib::InlineActionBehavior([&] { dummy6TickCount++; return gamelib::BehaviorResult::Success; });
	auto* DummyAction7 = new gamelib::InlineActionBehavior([&]{ dummy7TickCount++; return gamelib::BehaviorResult::Success; });
	auto* DummyAction8 = new gamelib::InlineActionBehavior([&] { dummy8TickCount++; return gamelib::BehaviorResult::Success; });

	gamelib::BehaviorTree* behaviorTree = BehaviorTreeBuilder()
		.ActiveSelector()
			.Sequence("Check if player is visible")
				.Condition(IsPlayerVisible)
			    .ActiveSelector()
					.Sequence()
						.Condition(IsPlayerInRange)
						.Filter()
							.Action(FireAtPlayer)
	                    .Finish() // finish filter sequence
	                .Finish() // finish sequence
					.Action(MoveTowardsPlayer)
	            .Finish() // finish active selector
	        .Finish() // finish sequence
            .Sequence("Check if player spotted")
				.Condition(HaveWeGotASuspectedLocation)
			    .Action(MovePlayerToLastKnownLocation)
				.Action(LookAround)
			.Finish() // finish sequence
			.Sequence("Act normal")
				.Action(MoveToRandomPosition)
				.Action(LookAroundSomeMore)
				.Finish() // finish sequence
			.Sequence("Do something else")
				.Action(DummyAction7)
				.Action(DummyAction8)
			.Finish() // finish sequence
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