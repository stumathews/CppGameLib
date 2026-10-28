#include "pch.h"
#include "audio/AudioManager.h"
#include "events/EventManager.h"
#include "graphic/SDLGraphicsManager.h"
#include <gtest/gtest.h>

using namespace std;
namespace gamelib
{
	namespace
	{
		class SDLGraphicsManagerTests : public testing::Test 
		{
		protected:

			void SetUp() override
			{
			}
	    
			void TearDown() override {}
		
		};
	}

	TEST_F(SDLGraphicsManagerTests, Initialize)
	{
		EXPECT_TRUE(SdlGraphicsManager::Get()->Initialize());
	}
}
