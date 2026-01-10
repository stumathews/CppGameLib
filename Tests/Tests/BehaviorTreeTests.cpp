#include "pch.h"
#include "ai/BehaviorTree.h"
#include "ai/ActiveSelector.h"
#include "ai/InlineAction.h"
#include "ai/Sequence.h"
#include "file/Logger.h"
#include <gmock/gmock-more-matchers.h>

#include "ai/ScriptedBehavior.h"
#include "ai/UtilitySelector.h"

using namespace std;
namespace gamelib
{
	class BehaviorTreeTests : public testing::Test 
	{
	 protected:

		void SetUp() override
		{
			gamelib::ResourceManager::Get()->Initialize("./Resources.xml");
		}

		void TearDown() override
		{
			gamelib::ResourceManager::Get()->Unload();
		}
	};


	TEST_F(BehaviorTreeTests, Behavior_Tree_Does_Not_Throw)
	{	
		bool crawled = false, walked = false, ran = false;

		ai::InlineAction crawl([&](const unsigned long deltaMs) { crawled = true; return Status::Success; });
		ai::InlineAction walk([&](const unsigned long deltaMs) { walked = true; return Status::Success; });
		ai::InlineAction run([&](const unsigned long deltaMs) { ran = true; return Status::Success; });

		ActiveSelector root;
		root.AddChild(&crawl);
		root.AddChild(&walk);
		root.AddChild(&run);
		
		// When creating behavior tree, ensure it does not throw
		const BehaviorTree behaviorTree(&root);

		// Ensure the basic tick doesn't throw either
		EXPECT_NO_THROW(behaviorTree.Update());

		// Ensure 
		EXPECT_THAT(crawled, testing::IsTrue());
		EXPECT_THAT(walked, testing::IsFalse());
		EXPECT_THAT(ran, testing::IsFalse());
	}

	TEST_F(BehaviorTreeTests, ScriptedBehavior_Test)
	{
		const auto behavior = std::make_shared<ScriptedBehavior>("TestScript");

		behavior->OnInitialize();
		EXPECT_NO_THROW(behavior->Update(0));
	}

	TEST_F(BehaviorTreeTests, UtilitySelector)
	{
		UtilitySelector selector;

		constexpr auto behavior1Utility = 0.1f;
		constexpr auto behavior2Utility = 0.2f;
		constexpr auto behavior3Utility = 0.9f;

		int lastRunBehavior = 0;

		// Some behaviours with arbitrary utility values
		const auto behavior1 = new ai::InlineAction([&](unsigned long deltaMs) {
			std::cout << "Running Behavior 1\n";
			lastRunBehavior = 1;
			return Status::Success	;
		}, "", [&]{ return behavior1Utility; });

		const auto behavior2 = new ai::InlineAction([&](unsigned long deltaMs) {
			std::cout << "Running Behavior 2\n";
			lastRunBehavior = 2;
			return Status::Failure;
		}, "", [&]{ return behavior2Utility; });

		const auto behavior3 = new ai::InlineAction([&](unsigned long deltaMs) {
			std::cout << "Running Behavior 3\n";
			lastRunBehavior = 3;
			return Status::Failure;
		}, "", [&]{ return behavior3Utility; });

		selector.AddChild(behavior1);
		selector.AddChild(behavior2);
		selector.AddChild(behavior3);

		selector.OnInitialize();

		// When selecting the next behavior
		selector.Update(0);

		// Ensure the children are evaluated from the child with the highest utility to the lowest
		const auto children = selector.GetChildren();

		ASSERT_EQ(children[0]->CalculateUtility(), behavior3Utility); // first run behaviour should be behaviour 3 (largest utility)
		ASSERT_EQ(children[1]->CalculateUtility(), behavior2Utility); // next highest utility
		ASSERT_EQ(children[2]->CalculateUtility(), behavior1Utility); // lowest utility

		ASSERT_EQ(lastRunBehavior, 1);

		// Check if the composite node type correctly calculates its utility as the largest utility of its children
		ASSERT_EQ(selector.CalculateUtility(), behavior3Utility);

		// Run the selector one more time and ensure that only the behaviour 1 was run (as it returned success)
		selector.Update(0);

		ASSERT_EQ(lastRunBehavior, 1);

	}
}