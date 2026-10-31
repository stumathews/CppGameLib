#include "pch.h"
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>
#include <gtest/gtest.h>

#include "events/DeterministicEventScheduler.h"

using namespace std;

namespace gamelib
{
    class DeterministicEventScedulerTests : public testing::Test
    {
    protected:

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F(DeterministicEventScedulerTests, SimpleUsage)
    {
        gamelib::DeterministicEventScheduler scheduler;

        auto tick1FunctionRan = false;
        auto tick5FunctionRan = false;
        auto tick10FunctionRan = false;

        // Schedule some events to occur at very specific times (ticks)
        scheduler.Schedule(1, [&]() {
            // some work
            tick1FunctionRan = true;
        });

        scheduler.Schedule(5, [&]() {
            // some work
            tick5FunctionRan = true;
        });

        scheduler.Schedule(10, [&]() {
            // some work
            tick10FunctionRan = true;
        });

        EXPECT_EQ(false, tick1FunctionRan);
        EXPECT_EQ(false, tick5FunctionRan);
        EXPECT_EQ(false, tick10FunctionRan);

        scheduler.Update(1);

        EXPECT_EQ(true, tick1FunctionRan);
        EXPECT_EQ(false, tick5FunctionRan);
        EXPECT_EQ(false, tick10FunctionRan);

        scheduler.Update(2);
        scheduler.Update(3);
        scheduler.Update(4);
        scheduler.Update(5);

        EXPECT_EQ(true, tick1FunctionRan);
        EXPECT_EQ(true, tick5FunctionRan);
        EXPECT_EQ(false, tick10FunctionRan);

        scheduler.Update(6);
        scheduler.Update(7);
        scheduler.Update(8);
        scheduler.Update(9);
        scheduler.Update(10);

        EXPECT_EQ(true, tick1FunctionRan);
        EXPECT_EQ(true, tick5FunctionRan);
        EXPECT_EQ(true, tick10FunctionRan);

        scheduler.Update(11);

    }
}//
// Created by stuart on 10/31/26.
//
