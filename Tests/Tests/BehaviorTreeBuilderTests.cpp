#include "pch.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "ai/BehaviorTree.h"
#include "ai/BehaviorTreeBuilder.h"
#include "ai/InlineAction.h"
#include "ai/Repeat.h"

using namespace std;

namespace gamelib {
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
		auto dummyAction5Result = Status::Success;

		auto* checkIsPlayerVisible = new ai::InlineAction ([&](const unsigned long deltaMs)
		{
			return isPlayerVisible
					? Status::Success
					: Status::Failure;
		}, "IsPlayerVisible");

		auto* checkPlayerInRange = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			return isPlayerInRange
					? Status::Success
					: Status::Failure;
		}, "IsPlayerInRange");

		auto* checkHaveWeGotASuspectedLocation = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			return haveWeGotASuspectedLocation
					? Status::Success
					: Status::Failure;
		});

		auto* doFireAtPlayer = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			fireAtPlayer++;
			return Status::Success;
		}, "FireAtPlayer");

		auto* doMoveTowardsPlayer = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			moveTowardsPlayer++;
			return Status::Success;
		});
		auto* doMovePlayerToLastKnownLocation = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			movePlayerToLastKnownLocation++;
			return Status::Success;
		}, "MovePlayerToLastKnownLocation");
		auto* doLookAround = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			lookAround++;
			return Status::Success;
		}, "LookAround");
		auto* doMoveToRandomPosition = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			moveToRandomPosition++;
			return dummyAction5Result;
		}, "MoveToRandomPosition");
		auto* doLookAroundSomeMore = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			lookAroundSomeMore++;
			return Status::Success;
		});
		auto* doDummyAction7 = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			dummyAction7++;
			return Status::Success;
		}, "DummyAction7");
		auto* doDummyAction8 = new ai::InlineAction([&](const unsigned long deltaMs)
		{
			dummyAction8++;
			return Status::Success;
		}, "DummyAction8");

		BehaviorTree* behaviorTree = BehaviorTreeBuilder()
			.ActiveSelector() // AS will always start from the highest priority child on each update, effectively re-evaluating past decisions
				.Sequence("Check if player is visible")
					.Condition(checkIsPlayerVisible)
					.ActiveSelector()
						.Sequence("")
							.Condition(checkPlayerInRange)
							.Sequence()
								.Action(doFireAtPlayer)
							.Finish()
						.Finish()
					.Action(doMoveTowardsPlayer)
					.Finish()
				.Finish()
				.Sequence("Check if player spotted")
					.Condition(checkHaveWeGotASuspectedLocation)
					.Action(doMovePlayerToLastKnownLocation)
					.Action(doLookAround)
				.Finish()
				.Sequence("Act normal")
					.Action(doMoveToRandomPosition)
					.Action(doLookAroundSomeMore)
				.Finish()
				.Sequence("Do something else")
					.Action(doDummyAction7)
					.Action(doDummyAction8)
				.Finish()
			.Finish()
		.Finish()
		.End();

		// Player is in range...

		behaviorTree->Update();
		behaviorTree->Update();
		EXPECT_THAT(fireAtPlayer, testing::Eq(2));

		// As the ActiveNodeSelector will repeat the last successful child (IsPlayerInRange & FireAtPLayer)
		// so will not execute the next child (move towards player)
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(0));

		// Now set player is not in Range, it will fail the sequence and move to the next child in ActiveNodeSelector
		isPlayerInRange = false;
				
		behaviorTree->Update();
		behaviorTree->Update();
		behaviorTree->Update();

		// Still expect that only the two firing behaviors were made (none more)
		EXPECT_THAT(fireAtPlayer, testing::Eq(2));

		// But now we've moved towards the player 3 times
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(3));

		// Now make the player no longer visible,
		// should fail the current sequence root and move to the next child in the ActiveSelector, i.e check if player is spotted
		isPlayerVisible = false;
		behaviorTree->Update();
		behaviorTree->Update();
		EXPECT_THAT(fireAtPlayer, testing::Eq(2)); // still the same
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(3)); // still the same

		// The two consecutive actions in the sequence should run
		EXPECT_THAT(movePlayerToLastKnownLocation, testing::Eq(2)); // now we call this action for two ticks
		EXPECT_THAT(lookAround, testing::Eq(2)); // now we call this action for two ticks

		// Now fail that sequence by returning failing for the condition, HaveWeGotASuspectedLocation
		haveWeGotASuspectedLocation = false;
		behaviorTree->Update();
		behaviorTree->Update();
		behaviorTree->Update();
		behaviorTree->Update();
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
		dummyAction5Result = Status::Failure;
		behaviorTree->Update();
		behaviorTree->Update();
		behaviorTree->Update();
		behaviorTree->Update();
		EXPECT_THAT(dummyAction7, testing::Eq(4));
		EXPECT_THAT(dummyAction8, testing::Eq(4));

		// Now test if the active selector works well by re-executing the nodes
		isPlayerVisible = true;
		isPlayerInRange = true;
		behaviorTree->Update();
		EXPECT_THAT(fireAtPlayer, testing::Eq(3));

		// Now disable player in range so that the sequence quits and move to the next behavior
		isPlayerInRange = false;
		behaviorTree->Update();
		EXPECT_THAT(moveTowardsPlayer, testing::Eq(4));

	}

	TEST_F(BehaviorTreeBuilderTests, Test_NotCondition)
	{
		auto count = 0;
		const auto success  = new ai::InlineAction([](unsigned long deltaMs) { return Status::Success; });
		const auto notSuccess  = new ai::InlineAction([](unsigned long deltaMs) { return Status::Failure; });
		const auto action  = new ai::InlineAction([&count](unsigned long deltaMs) {
			count++;
			return Status::Success;
		});

		const auto tree = BehaviorTreeBuilder()
			.ActiveSelector()
				.Sequence("Check if player is visible")
					.Condition(success) // This should succeed
					.Not(notSuccess) // This should invert the failure to success
					.Action(action) // This should execute
				.Finish()
			.Finish()
		.End();

		// All conditions in the sequence should not fail and so action should be executed
		tree->Update(0);
		tree->Update(0);
		tree->Update(0);

		// 3 times for each update
		ASSERT_EQ(count, 3);
	}

	TEST_F(BehaviorTreeBuilderTests, TestWaterfallFallback) {

		/* Waterfall Fallback (failed behaviors, fail down to the next behavior etc., never going back up the waterfall)
		 *
		 * A selector will continuously repeat non-failing behaviors.
		 * On failure, the next behavior is continuously run.
		 * When the last behavior fails, the selector returns failure.
		 *
		 */


		bool walking = false;
		bool aiming = false;
		bool shooting = false;
		bool stopWalking = false;
		bool stopAiming= false;
		bool stopShooting = false;

		std::vector<std::string> sequence;

		auto walk = ai::InlineAction([&](unsigned long deltaMs) {
			if (stopWalking)
			{
				std::cout << "been asked to stop walking\n";
				sequence.push_back("stopwalking");
				return Status::Failure;
			}
			std::cout << "walking\n";
			walking = true;
			sequence.push_back("walking");
			return Status::Running;
		});

		auto aim = ai::InlineAction([&](unsigned long deltaMs) {
			if (stopAiming) {
				std::cout << "been asked to stop aiming\n";
				sequence.push_back("stopaiming");
				return Status::Failure;
			}
			std::cout << "aiming\n";
			aiming = true;
			sequence.push_back("aiming");
			return Status::Running;
		});

		auto shoot = ai::InlineAction([&](unsigned long deltaMs) {
			if (stopShooting) {
				std::cout << "been asked to stop shooting\n";
				sequence.push_back("stopshooting");
				return Status::Failure;
			}
			std::cout << "shooting\n";
			shooting = true;
			sequence.push_back("shooting");
			return Status::Running;
		});

		BehaviorTree* behaviorTree = BehaviorTreeBuilder()
			.Selector()
				.Action(&walk)
				.Action(&aim)
				.Action(&shoot)
			.Finish()
		.End();

		EXPECT_FALSE(walking);
		EXPECT_FALSE(aiming);
		EXPECT_FALSE(shooting);

		behaviorTree->Update(0); // walking
		behaviorTree->Update(0); // walking
		stopWalking = true;
		behaviorTree->Update(0); // aiming (walking does nothing)
		behaviorTree->Update(0); // aiming (walking does nothing)
		behaviorTree->Update(0); // aiming (walking does nothing)
		stopAiming = true;
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)
		stopShooting = true;
		behaviorTree->Update(0); // nothing is run

		EXPECT_TRUE(std::string("walking") == sequence.at(0));
		EXPECT_TRUE(std::string("walking") == sequence.at(1));
		EXPECT_TRUE(std::string("stopwalking") == sequence.at(2));
		EXPECT_TRUE(std::string("aiming") == sequence.at(3));
		EXPECT_TRUE(std::string("aiming") == sequence.at(4));
		EXPECT_TRUE(std::string("aiming") == sequence.at(5));
		EXPECT_TRUE(std::string("stopaiming") == sequence.at(6));
		EXPECT_TRUE(std::string("shooting") == sequence.at(7));
		EXPECT_TRUE(std::string("shooting") == sequence.at(8));
		EXPECT_TRUE(std::string("shooting") == sequence.at(9));
		EXPECT_TRUE(std::string("shooting") == sequence.at(10));
		EXPECT_TRUE(std::string("stopshooting") == sequence.at(11));
		EXPECT_EQ(sequence.size(), 12);
	}

	TEST_F(BehaviorTreeBuilderTests, TestTeleportationFallback) {
		/* Teleportation Fallback (When behavior fails, don't fallback to next behavior down the waterfall, teleport to the top again)
		 *
		 * A ActiveSelector will continuously repeat non-failing behaviors.
		 * On failure, the ActiveSelector restarts
		 */

		bool walking = false;
		bool aiming = false;
		bool shooting = false;
		bool stopWalking = false;
		bool stopAiming= false;
		bool stopShooting = false;

		std::vector<std::string> sequence(0);

		auto walk = ai::InlineAction([&](unsigned long deltaMs) {
			if (stopWalking)
			{
				std::cout << "been asked to stop walking\n";
				sequence.push_back("stopwalking");
				return Status::Failure;
			}
			std::cout << "walking\n";
			sequence.push_back("walking");
			walking = true;
			return Status::Running;
		});

		auto aim = ai::InlineAction([&](unsigned long deltaMs) {
			if (stopAiming) {
				std::cout << "been asked to stop aiming\n";
				sequence.push_back("stopaiming");
				return Status::Failure;
			}
			std::cout << "aiming\n";
			sequence.push_back("aiming");
			aiming = true;
			return Status::Running;
		});

		auto shoot = ai::InlineAction([&](unsigned long deltaMs) {
			if (stopShooting) {
				std::cout << "been asked to stop shooting\n";
				sequence.push_back("stopshooting");
				return Status::Failure;
			}
			std::cout << "shooting\n";
			sequence.push_back("shooting");
			shooting = true;
			return Status::Running;
		});

		BehaviorTree* behaviorTree = BehaviorTreeBuilder()
			.ActiveSelector()
				.Action(&walk)
				.Action(&aim)
				.Action(&shoot)
			.Finish()
		.End();

		EXPECT_FALSE(walking);
		EXPECT_FALSE(aiming);
		EXPECT_FALSE(shooting);

		behaviorTree->Update(0); // walking
		behaviorTree->Update(0); // walking
		EXPECT_TRUE(std::string("walking") == sequence.at(0));
		EXPECT_TRUE(std::string("walking") == sequence.at(1));
		stopWalking = true;
		behaviorTree->Update(0); // aiming (walking does nothing)
		behaviorTree->Update(0); // aiming (walking does nothing)
		behaviorTree->Update(0); // aiming (walking does nothing)
		EXPECT_TRUE(std::string("stopwalking") == sequence.at(2));
		EXPECT_TRUE(std::string("aiming") == sequence.at(3));
		EXPECT_TRUE(std::string("stopwalking") == sequence.at(4)); // teleport
		EXPECT_TRUE(std::string("aiming") == sequence.at(5));
		EXPECT_TRUE(std::string("stopwalking") == sequence.at(6)); // teleport
		EXPECT_TRUE(std::string("aiming") == sequence.at(7));

		stopAiming = true;
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)
		EXPECT_TRUE(std::string("stopwalking") == sequence.at(8)); // teleport
		EXPECT_TRUE(std::string("stopaiming") == sequence.at(9));
		EXPECT_TRUE(std::string("shooting") == sequence.at(10));
		EXPECT_TRUE(std::string("stopwalking") == sequence.at(11)); // teleport
		EXPECT_TRUE(std::string("stopaiming") == sequence.at(12));
		EXPECT_TRUE(std::string("shooting") == sequence.at(13));
		EXPECT_TRUE(std::string("stopwalking") == sequence.at(14)); // teleport
		EXPECT_TRUE(std::string("stopaiming") == sequence.at(15));
		EXPECT_TRUE(std::string("shooting") == sequence.at(16));
		EXPECT_TRUE(std::string("stopwalking") == sequence.at(17)); // teleport
		EXPECT_TRUE(std::string("stopaiming") == sequence.at(18));
		EXPECT_TRUE(std::string("shooting") == sequence.at(19));
		stopShooting = true;
		behaviorTree->Update(0); // shooting (walking does nothing, aiming does nothing)

		EXPECT_EQ(sequence.size(),23);
	}


}