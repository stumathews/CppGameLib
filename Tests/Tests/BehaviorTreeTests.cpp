#include "pch.h"
#include "ai/BehaviorTree.h"
#include "ai/FirstOrNextSelector.h"
#include "ai/InlineActionBehavior.h"
#include "ai/Sequence.h"
#include "file/Logger.h"
#include <gmock/gmock-more-matchers.h>

using namespace std;
namespace gamelib
{
	class BehaviorTreeTests : public testing::Test 
	{
	 protected:

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};


	TEST_F(BehaviorTreeTests, Behavior_Tree_Does_Not_Throw)
	{	
		bool crawled = false, walked = false, ran = false;

		InlineActionBehavior crawl([&] { crawled = true; return BehaviorResult::Success; });
		InlineActionBehavior walk([&] { walked = true; return BehaviorResult::Success; });
		InlineActionBehavior run([&] { ran = true; return BehaviorResult::Success; });

		FirstOrNextSelector root;
		root.AddChild(&crawl);
		root.AddChild(&walk);
		root.AddChild(&run);
		
		// When creating behavior tree, ensure it does not throw
		BehaviorTree behaviorTree(&root);

		// Ensure the basic tick doesn't throw either
		EXPECT_NO_THROW(behaviorTree.Tick());

		// Ensure 
		EXPECT_THAT(crawled, testing::IsTrue());
		EXPECT_THAT(walked, testing::IsFalse());
		EXPECT_THAT(ran, testing::IsFalse());
	}
}