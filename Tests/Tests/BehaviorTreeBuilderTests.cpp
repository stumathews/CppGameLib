#include "pch.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "ai/BehaviorTree.h"
#include "ai/BehaviorTreeBuilder.h"
#include "ai/InlineBehavioralAction.h"
#include "ai/Repeat.h"

using namespace std;

namespace gamelib
{
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
		auto fireAtPlayer = 0, moveTowardsPlayer = 0, movePlayerToLastKnownLocation = 0, lookAround = 0,
			moveToRandomPosition = 0, lookAroundSomeMore = 0, dummyAction7 = 0, dummyAction8 = 0;

		bool isPlayerVisible = true;
		bool isPlayerInRange = true;
		bool haveWeGotASuspectedLocation = true;
		auto dummyAction5Result = gamelib::BehaviorResult::Success;

		auto* checkIsPlayerVisible = new gamelib::InlineBehavioralAction ([&](const unsigned long deltaMs)
		{
			return isPlayerVisible
					? gamelib::BehaviorResult::Success
					: gamelib::BehaviorResult::Failure;
		}, "IsPlayerVisible");

		auto* checkPlayerInRange = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			return isPlayerInRange
					? gamelib::BehaviorResult::Success
					: gamelib::BehaviorResult::Failure;
		}, "IsPlayerInRange");

		auto* checkHaveWeGotASuspectedLocation = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			return haveWeGotASuspectedLocation
					? gamelib::BehaviorResult::Success
					: gamelib::BehaviorResult::Failure;
		});

		auto* doFireAtPlayer = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			fireAtPlayer++; 
			return gamelib::BehaviorResult::Success;
		}, "FireAtPlayer");

		auto* doMoveTowardsPlayer = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			moveTowardsPlayer++;
			return gamelib::BehaviorResult::Success;
		});
		auto* doMovePlayerToLastKnownLocation = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			movePlayerToLastKnownLocation++;
			return gamelib::BehaviorResult::Success;
		}, "MovePlayerToLastKnownLocation");
		auto* doLookAround = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			lookAround++;
			return gamelib::BehaviorResult::Success;
		}, "LookAround");
		auto* doMoveToRandomPosition = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			moveToRandomPosition++;
			return dummyAction5Result;
		}, "MoveToRandomPosition");
		auto* doLookAroundSomeMore = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			lookAroundSomeMore++;
			return gamelib::BehaviorResult::Success;
		});
		auto* doDummyAction7 = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			dummyAction7++;
			return gamelib::BehaviorResult::Success;
		}, "DummyAction7");
		auto* doDummyAction8 = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			dummyAction8++;
			return gamelib::BehaviorResult::Success;
		}, "DummyAction8");

		gamelib::BehaviorTree* behaviorTree = BehaviorTreeBuilder()
			.ActiveNodeSelector() //1
				.Sequence("Check if player is visible") //2
					.Condition(checkIsPlayerVisible) // 3
				    .ActiveNodeSelector() // 4
						.Sequence("") // 5
							.Condition(checkPlayerInRange) // 6
							.Sequence() // 7 (this sequence will repeat while player is in range)
								.Action(doFireAtPlayer) // 8
		                    .Finish() // finish sequence 7
		                .Finish() // finish sequence 5
		            .Action(doMoveTowardsPlayer) // 9
		            .Finish() // finish active selector 4
		        .Finish() // finish sequence 2
	            .Sequence("Check if player spotted") //10
					.Condition(checkHaveWeGotASuspectedLocation) //11
				    .Action(doMovePlayerToLastKnownLocation) //12
					.Action(doLookAround) //13
				.Finish() // finish sequence
				.Sequence("Act normal") //14
					.Action(doMoveToRandomPosition) //15
					.Action(doLookAroundSomeMore) //16
					.Finish() // finish sequence
				.Sequence("Do something else") // 17
					.Action(doDummyAction7)  //18
					.Action(doDummyAction8) //19
				.Finish() // finish sequence
			.Finish()
		.Finish()
		.End();

		// Player is in range...

		behaviorTree->Tick();
		behaviorTree->Tick();
		EXPECT_THAT(fireAtPlayer, testing::Eq(2));

		// As the ActiveNodeSelector will repeat the last successful child (IsPlayerInRange & FireAtPLayer)
		// so will not execute the next child (move towards player)
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(0));

		// Now set player is not in Range, it will fail the sequence and move to the next child in ActiveNodeSelector
		isPlayerInRange = false;
				
		behaviorTree->Tick();
		behaviorTree->Tick();
		behaviorTree->Tick();

		// Still expect that only the two firing behaviors were made (none more)
		EXPECT_THAT(fireAtPlayer, testing::Eq(2));

		// But now we've moved towards the player 3 times
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(3));

		// Now make the player no longer visible,
		// should fail the current sequence root and move to the next child in the ActiveSelector, i.e check if player is spotted
		isPlayerVisible = false;
		behaviorTree->Tick();
		behaviorTree->Tick();
		EXPECT_THAT(fireAtPlayer, testing::Eq(2)); // still the same
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(3)); // still the same

		// The two consecutive actions in the sequence should run
		EXPECT_THAT(movePlayerToLastKnownLocation, testing::Eq(2)); // now we call this action for two ticks
		EXPECT_THAT(lookAround, testing::Eq(2)); // now we call this action for two ticks

		// Now fail that sequence by returning failing for the condition, HaveWeGotASuspectedLocation
		haveWeGotASuspectedLocation = false;
		behaviorTree->Tick();
		behaviorTree->Tick();
		behaviorTree->Tick();
		behaviorTree->Tick();
		EXPECT_THAT(fireAtPlayer, testing::Eq(2)); // still same
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(3)); // still same
		EXPECT_THAT(movePlayerToLastKnownLocation, testing::Eq(2)); //still same
		EXPECT_THAT(lookAround, testing::Eq(2)); // still same

		// Now the next sequence should run, i.e Act normal
		EXPECT_THAT(moveToRandomPosition, testing::Eq(4));
		EXPECT_THAT(lookAroundSomeMore, testing::Eq(4));

		// Because the previous child is successful, it continues to run because its within a ActiveSelector
		EXPECT_THAT(dummyAction7, testing::Eq(0));
		EXPECT_THAT(dummyAction8, testing::Eq(0));
		
		// Now make the previous sequence fail and we should run...
		dummyAction5Result = gamelib::BehaviorResult::Failure;
		behaviorTree->Tick();
		behaviorTree->Tick();
		behaviorTree->Tick();
		behaviorTree->Tick();
		EXPECT_THAT(dummyAction7, testing::Eq(4));
		EXPECT_THAT(dummyAction8, testing::Eq(4));

		// Now test if the active selector works well by re-executing the nodes
		isPlayerVisible = true;
		isPlayerInRange = true;
		behaviorTree->Tick();
		EXPECT_THAT(fireAtPlayer, testing::Eq(3));

		// Now disable player in range so that the sequence quits and move to the next behavior
		isPlayerInRange = false;
		behaviorTree->Tick();
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(4));

	}
}