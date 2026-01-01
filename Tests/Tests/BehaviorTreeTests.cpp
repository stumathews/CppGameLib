#include "pch.h"
#include "ai/BehaviorTree.h"
#include "ai/ActiveSelector.h"
#include "ai/InlineAction.h"
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
}