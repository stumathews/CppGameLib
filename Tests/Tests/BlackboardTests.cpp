#include <thread>
#include <gtest/gtest.h>
#include <chrono>
using namespace std::chrono_literals;
#include "ai/Blackboard.h"
#include "ai/BlackboardKeyRegistry.h"

namespace gamelib
{
    class BlackboardTests : public testing::Test {
    public:

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };



    TEST_F(BlackboardTests, Test1)
    {
        // Declare/Registry blackboard keys
        BB_DECLARE_KEY(uint64_t, K_TargetId);
        BB_DECLARE_KEY(bool,      K_EnemyVisible);
        BB_DECLARE_KEY(float,     K_EnemyLastSeenX);
        BB_DECLARE_KEY(float,     K_EnemyLastSeenY);
        BB_DECLARE_KEY(float,     K_EnemyLastSeenZ);
        BB_DECLARE_KEY(int,       K_Health);

        // Create a couple of blackboards
        Blackboard global;   // global scope
        Blackboard squad;    // squad scope
        Blackboard agent;    // agent scope

        // Allow a hierarchy of lookups if not found in blackboard initially queried
        squad.SetParent(&global);
        agent.SetParent(&squad);

        // Keys to use
        K_EnemyVisible keyEnemyVisible;
        K_Health keyHealth;
        K_EnemyLastSeenX keyLastSeenX;
        K_EnemyLastSeenY keyLastSeenY;

        // Check key name stored in meta-data correctly
        ASSERT_STREQ(std::string(keyEnemyVisible.Name()).c_str(), "K_EnemyVisible");

        // Write to the squad blackboard
        squad.Set(&keyLastSeenY,13.5f, Writer::Perception, /*ttl*/ 0.3f);

        // Write to the agent blackboard
        agent.Set(&keyEnemyVisible, true, Writer::Perception, /*ttl*/ 0.3f);
        agent.Set(&keyHealth, 25, Writer::Perception, /*ttl*/ 0.3f);
        agent.Set(&keyLastSeenX, 12.5f, Writer::Perception, /*ttl*/ 0.4f);

        // Subscribe to changes in the agent's blackboard for the keyEnemyVisible property/fact
        int changedCallbackUsedTimes = 0;
        const int subscription = agent.Subscribe(keyEnemyVisible.Id(), [&](BlackboardKeyId k)
        {
            if (const auto v = agent.Get<bool>(&keyEnemyVisible))
            {
                // Make sure the set value is the detected change used in the callback
                ASSERT_EQ(*v, true);
                changedCallbackUsedTimes++;
            }
        });

        // Update the blackboards with time
        agent.Update(0.016f); // apply staged writes
        squad.Update(0.016f); // apply staged writes
        global.Update(0.016f); // apply staged writes

        // Fetch the stored values in blackboard
        auto enemyVisible = agent.Get(&keyEnemyVisible);
        auto health = agent.Get(&keyHealth);
        auto lastSeenX = agent.Get(&keyLastSeenX);
        auto lastSeenY = agent.Get(&keyLastSeenY);

        // Ensure what we put in is what we get out
        ASSERT_EQ(*enemyVisible, true);
        ASSERT_EQ(*health, 25);
        ASSERT_EQ(*lastSeenX, 12.5f);
        ASSERT_EQ(*lastSeenY, 13.5f); // Should get it from parent
        ASSERT_EQ(changedCallbackUsedTimes, 1); // Make sure subscription work

        // Let some values expire
        global.Update(0.284);
        squad.Update(0.284);
        agent.Update(0.284);

        // Fetch the stored values in blackboard
        enemyVisible = agent.Get(&keyEnemyVisible);
        health = agent.Get(&keyHealth);
        lastSeenX = agent.Get(&keyLastSeenX);
        lastSeenY = agent.Get(&keyLastSeenY);

        ASSERT_EQ(enemyVisible, nullptr);
        ASSERT_EQ(health, nullptr);
        ASSERT_EQ(lastSeenY, nullptr);
        ASSERT_TRUE(lastSeenX != nullptr); // Hasn't expired yet
        ASSERT_EQ(*lastSeenX, 12.5f); // Should still have its value
        ASSERT_EQ(changedCallbackUsedTimes, 1); // Make sure subscription work

        agent.Unsubscribe(subscription);

        // See if the unsubscribe operation worked...

        // Set the value (should not cause callback to run)
        agent.Set(&keyEnemyVisible, false);

        global.Update(0.284);
        squad.Update(0.284);
        agent.Update(0.284);

        // Ensure that it did not run a second time
        ASSERT_EQ(changedCallbackUsedTimes, 1); // Make sure subscription work

    }
}