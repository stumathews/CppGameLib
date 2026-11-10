//
// Created by stuart on 11/10/26.
//

#include <gtest/gtest.h>

#include "character/AnimatedSprite.h"

namespace gamelib
{

    class AnimatedSpriteTestFixture : public ::testing::Test
    {

    };

    TEST(AnimatedSpriteTest, DisableEnableAnimation)
    {
        // Create simple animated sprite with 4 key frames
        AnimatedSprite sprite;
        sprite.AddKeyFrame({0,0,0,0,"up"});
        sprite.AddKeyFrame({0,0,0,0,"down"});
        sprite.AddKeyFrame({0,0,0,0,"left"});
        sprite.AddKeyFrame({0,0,0,0,"right"});

        // simulate a frame being shown for 10 ms each
        sprite.SetSingleFrameDuration(10);

        EXPECT_EQ(sprite.KeyFrames.size(), 4);

        // By default, the animation is started
        EXPECT_FALSE(sprite.IsStopped());

        sprite.DisableAnimation();
        EXPECT_TRUE(sprite.IsStopped());

        // When enabling it, ensure it is not stopped anymore
        sprite.EnableAnimation();
        EXPECT_FALSE(sprite.IsStopped());

        // Ensure the initial frame key is index 0
        const auto frameKeyNumber = sprite.GetCurrentKeyFrameIndex();
        EXPECT_EQ(frameKeyNumber, 0);

        // Simulate 16 ms frames
        constexpr auto deltaTime = DeltaTime(16);

        // Update, the key frame index should change (it exceeded its duration of 10ms)
        sprite.Update(&deltaTime);
        EXPECT_EQ(sprite.GetCurrentKeyFrameIndex(), 1);

        sprite.Update(&deltaTime);
        EXPECT_EQ(sprite.GetCurrentKeyFrameIndex(), 2);

        sprite.Update(&deltaTime);
        EXPECT_EQ(sprite.GetCurrentKeyFrameIndex(), 3);

        // Now disable animations
        sprite.DisableAnimation();

        // Update, the key frame index should not change (it should remain constant)
        sprite.Update(&deltaTime);
        EXPECT_EQ(sprite.GetCurrentKeyFrameIndex(), 3);
        sprite.Update(&deltaTime);
        EXPECT_EQ(sprite.GetCurrentKeyFrameIndex(), 3);
        sprite.Update(&deltaTime);
        EXPECT_EQ(sprite.GetCurrentKeyFrameIndex(), 3);

        // Enable it, it should resume from where it left off ( index 3 is last frame, next frame should be 0 again (wrap around index))
        sprite.EnableAnimation();
        sprite.Update(&deltaTime);
        EXPECT_EQ(sprite.GetCurrentKeyFrameIndex(), 0);
    }
}
