//
// Created by stuart on 11/11/26.
//

#include "pch.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "ai/BehaviorTree.h"
#include "ai/BehaviorTreeBuilder.h"
#include "ai/InlineAction.h"
#include "ai/Repeat.h"

using namespace std;

namespace gamelib
{
    class BasicBehaviorTreeTests : public testing::Test
    {
    protected:

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST(BasicBehaviorTreeTests, ActiveSelectorBehaviorTests)
    {
        auto action1Status = Status::Success;
        auto action2Status = Status::Success;

        std::vector<std::string> events;

        ai::InlineAction action1([&](unsigned long deltaMs)
        {
            std::cout << "Running Behavior 1\n";
            events.emplace_back("action1");
            return action1Status;
        });

        ai::InlineAction action2([&](unsigned long deltaMs)
        {
            std::cout << "Running Behavior 2\n";
            events.emplace_back("action2");
            return action2Status;
        });

        std::cout << "ActiveSelectorBehaviorTests:\n";

        BehaviorTree* bt = BehaviorTreeBuilder()
            .ActiveSelector()
                .Action(&action1)
                .Action(&action2)
            .Finish()
        .End();

        bt->Update(); // tick1
        EXPECT_EQ(events[0], "action1");  // tick1
        bt->Update(); // tick2
        EXPECT_EQ(events[1], "action1");  // tick2
        bt->Update(); // tick3
        EXPECT_EQ(events[2], "action1");  // tick2

        // Expect the active selector to keep running the non-failing child (action1)

        bt->Update(); // tick4
        EXPECT_EQ(events[3], "action1");
        bt->Update(); // tick5
        EXPECT_EQ(events[4], "action1");
        bt->Update(); // tick6
        EXPECT_EQ(events[5], "action1");

        std::cout << "Changing action 1 to failure \n";
        action1Status = Status::Failure;

        bt->Update(); // tick7
        EXPECT_EQ(events[6], "action1"); // action 1 runs but now it fails (tick7)
        EXPECT_EQ(events[7], "action2"); // action 2 runs immediately on failure action 1 failure (in the same tick 7)
        EXPECT_EQ(events.size(), 8);
        bt->Update(); // tick8
        EXPECT_EQ(events[8], "action1"); // action 1 is rechecked, fails (tick8)
        EXPECT_EQ(events[9], "action2"); // action 2 runs immediately in same tick8
        EXPECT_EQ(events.size(), 10);
        bt->Update(); // tick9
        EXPECT_EQ(events[10], "action1"); // action 1 is rechecked, fails(tick9)
        EXPECT_EQ(events[11], "action2"); // action 2 runs immediately in same tick9
        EXPECT_EQ(events.size(), 12);

        // Conclusion: Even on failure, the child is re-checked.
        // This enables sticky interrupting behavior is child becomes non-failure

        // Now if action 1 no longer is failing, its running, it should remain running
        // continuously - no action 2 is called.
        action1Status = Status::Running;

        bt->Update(); // tick10
        EXPECT_EQ(events[12], "action1"); // action 1 ticks as running
        bt->Update(); // tick11
        EXPECT_EQ(events[13], "action1"); // action 1 ticks as running
        bt->Update(); // tick11
        EXPECT_EQ(events[14], "action1"); // action 1 ticks as running

        // Success also still runs action 1 continuously
        action1Status = Status::Success;

        bt->Update(); // tick12
        EXPECT_EQ(events[15], "action1"); // action 1 ticks as running
        bt->Update(); // tick13
        EXPECT_EQ(events[16], "action1"); // action 1 ticks as running
        bt->Update(); // tick14
        EXPECT_EQ(events[17], "action1"); // action 1 ticks as running

        std::cout << "Changing action 1 to failure again:\n";
        action1Status = Status::Failure;

        bt->Update(); // tick15
        EXPECT_EQ(events[18], "action1"); // action 1 runs but now it fails (tick15)
        EXPECT_EQ(events[19], "action2"); // action 2 runs immediately on failure action 1 failure (in the same tick157)
        EXPECT_EQ(events.size(), 20);
        bt->Update(); // tick15
        EXPECT_EQ(events[20], "action1"); // action 1 is rechecked, fails (tick15)
        EXPECT_EQ(events[21], "action2"); // action 2 runs immediately in same tick15
        EXPECT_EQ(events.size(), 22);
        bt->Update(); // tick17
        EXPECT_EQ(events[22], "action1"); // action 1 is rechecked, fails(tick17)
        EXPECT_EQ(events[23], "action2"); // action 2 runs immediately in same tick17
        EXPECT_EQ(events.size(), 24);

        // Conclusion 2: The Non-failure behavior is sticky, it runs on each tick (success or running status)
        // On failure, failing behaviors are not skipped, but they are evaluated one at a time within the tick
        // and if all are still failure, they next behavior run in the tick.
    }

    TEST(BasicBehaviorTreeTests, DeltaTimeTest)
    {
        int elapsedTime = 0.0f;
        ai::InlineAction action([&](unsigned long deltaMs)
        {
            elapsedTime += deltaMs;
            return Status::Running;
        });

        std::cout << "SelectorBehaviorTests:\n";

        BehaviorTree* bt = BehaviorTreeBuilder()
            .Selector()
                .Action(&action)
            .Finish()
        .End();

        bt->Update(16.0f);
        bt->Update(16.0f);

        EXPECT_EQ(elapsedTime, 32.0f);

    }

    TEST(BasicBehaviorTreeTests, SelectorBehaviorTests)
    {
        auto action1Status = Status::Success;
        auto action2Status = Status::Success;

        std::vector<std::string> events;

        ai::InlineAction action1([&](unsigned long deltaMs)
        {
            std::cout << "Running Behavior 1\n";
            events.emplace_back("action1");
            return action1Status;
        });

        ai::InlineAction action2([&](unsigned long deltaMs)
        {
            std::cout << "Running Behavior 2\n";
            events.emplace_back("action2");
            return action2Status;
        });

        std::cout << "SelectorBehaviorTests:\n";

        BehaviorTree* bt = BehaviorTreeBuilder()
            .Selector()
                .Action(&action1)
                .Action(&action2)
            .Finish()
        .End();

        bt->Update(); // tick1
        EXPECT_EQ(events[0], "action1");  // tick1
        bt->Update(); // tick2
        EXPECT_EQ(events[1], "action1");  // tick2
        bt->Update(); // tick3
        EXPECT_EQ(events[2], "action1");  // tick2

        // Expect the active selector to keep running the non-failing child (action1)

        bt->Update(); // tick4
        EXPECT_EQ(events[3], "action1");
        bt->Update(); // tick5
        EXPECT_EQ(events[4], "action1");
        bt->Update(); // tick6
        EXPECT_EQ(events[5], "action1");

        std::cout << "Changing action 1 to failure \n";
        action1Status = Status::Failure;

        bt->Update(); // tick7
        EXPECT_EQ(events[6], "action1"); // action 1 runs but now it fails (tick7)
        EXPECT_EQ(events[7], "action2"); // action 2 runs immediately on failure action 1 failure (in the same tick 7)
        EXPECT_EQ(events.size(), 8);
        bt->Update(); // tick8
        EXPECT_EQ(events[8], "action1"); // action 1 is rechecked, fails (tick8)
        EXPECT_EQ(events[9], "action2"); // action 2 runs immediately in same tick8
        EXPECT_EQ(events.size(), 10);
        bt->Update(); // tick9
        EXPECT_EQ(events[10], "action1"); // action 1 is rechecked, fails(tick9)
        EXPECT_EQ(events[11], "action2"); // action 2 runs immediately in same tick9
        EXPECT_EQ(events.size(), 12);

        // Conclusion: Even on failure, the child is re-checked.
        // This enables sticky interrupting behavior is child becomes non-failure

        // Now if action 1 no longer is failing, its running, it should remain running
        // continuously - no action 2 is called.
        action1Status = Status::Running;

        bt->Update(); // tick10
        EXPECT_EQ(events[12], "action1"); // action 1 ticks as running
        bt->Update(); // tick11
        EXPECT_EQ(events[13], "action1"); // action 1 ticks as running
        bt->Update(); // tick11
        EXPECT_EQ(events[14], "action1"); // action 1 ticks as running

        // Success also still runs action 1 continuously
        action1Status = Status::Success;

        bt->Update(); // tick12
        EXPECT_EQ(events[15], "action1"); // action 1 ticks as running
        bt->Update(); // tick13
        EXPECT_EQ(events[16], "action1"); // action 1 ticks as running
        bt->Update(); // tick14
        EXPECT_EQ(events[17], "action1"); // action 1 ticks as running

        std::cout << "Changing action 1 to failure again:\n";
        action1Status = Status::Failure;

        bt->Update(); // tick15
        EXPECT_EQ(events[18], "action1"); // action 1 runs but now it fails (tick15)
        EXPECT_EQ(events[19], "action2"); // action 2 runs immediately on failure action 1 failure (in the same tick157)
        EXPECT_EQ(events.size(), 20);
        bt->Update(); // tick15
        EXPECT_EQ(events[20], "action1"); // action 1 is rechecked, fails (tick15)
        EXPECT_EQ(events[21], "action2"); // action 2 runs immediately in same tick15
        EXPECT_EQ(events.size(), 22);
        bt->Update(); // tick17
        EXPECT_EQ(events[22], "action1"); // action 1 is rechecked, fails(tick17)
        EXPECT_EQ(events[23], "action2"); // action 2 runs immediately in same tick17
        EXPECT_EQ(events.size(), 24);

        // Conclusion 2: The Non-failure behavior is sticky, it runs on each tick (success or running status)
        // On failure, failing behaviors are not skipped, but they are evaluated one at a time within the tick
        // and if all are still failure, they next behavior run in the tick.
    }
}