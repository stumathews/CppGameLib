#include "pch.h"
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>
#include <gtest/gtest.h>

#include "utils/Random.h"

using namespace std;

namespace gamelib
{
    class RandomnessTests : public testing::Test
    {
    protected:

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F(RandomnessTests, SimpleUsage)
    {
        // When using a random generator with a constant seed...
        gamelib::Random random(12345);
        auto integer1 = random.Int(1,10);
        auto integer2 = random.Int(1,10);
        auto integer3 = random.Int(1,10);
        auto integer4 = random.Int(1,10);
        auto integer5 = random.Int(1,10);

        // We expect the same sequence every time: 10, 9, 4 ,2, 2
        EXPECT_EQ(integer1, 10);
        EXPECT_EQ(integer2, 9);
        EXPECT_EQ(integer3, 4);
        EXPECT_EQ(integer4, 2);
        EXPECT_EQ(integer5, 2);

    }
}