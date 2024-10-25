#include "pch.h"
#include "audio/AudioManager.h"
#include "events/EventManager.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"
#include <gtest/gtest.h>

using namespace std;
namespace gamelib
{
	class SDLGraphicsManagerTests : public testing::Test 
	{
	 protected:

	  void SetUp() override
	  {
		event_admin = shared_ptr<EventManager>(EventManager::Get());
		font_admin = shared_ptr<FontManager>(FontManager::Get());
		graphics_admin = shared_ptr<SdlGraphicsManager>(SdlGraphicsManager::Get());
  		audio_admin = shared_ptr<AudioManager>(AudioManager::Get());
		resource_admin = shared_ptr<ResourceManager>(ResourceManager::Get());
  		scene_admin = shared_ptr<SceneManager>(SceneManager::Get());
		scene_admin->SetSceneFolder("");
	  }
	    
	  void TearDown() override {}
	  
	  shared_ptr<ResourceManager> resource_admin;
	  shared_ptr<EventManager> event_admin;	
	  shared_ptr<FontManager> font_admin;
	  shared_ptr<SdlGraphicsManager> graphics_admin;
	  shared_ptr<AudioManager> audio_admin;
	  shared_ptr<SceneManager> scene_admin;
	  shared_ptr<GameWorldData> world;
		
	};

	TEST_F(SDLGraphicsManagerTests, Initialize)
	{
		EXPECT_TRUE(graphics_admin->Initialize());
	}
}
