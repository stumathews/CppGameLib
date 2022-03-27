#include "pch.h"

#include "common/StaticConfig.h"
#include "events/EventManager.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "objects/GameObject.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"

using namespace std;
using namespace gamelib;

class SDLGraphicsManagerTests : public testing::Test 
{
 protected:

  void SetUp() override
  {  	
  	config = shared_ptr<SettingsManager>(SettingsManager::Get());
	event_admin = shared_ptr<EventManager>(EventManager::Get());
	font_admin = shared_ptr<FontManager>(FontManager::Get());
	graphics_admin = shared_ptr<SDLGraphicsManager>(SDLGraphicsManager::Get());
  	audio_admin = shared_ptr<AudioManager>(AudioManager::Get());
	resource_admin = shared_ptr<ResourceManager>(ResourceManager::Get());
	world = shared_ptr<GameWorldData>(new GameWorldData());
  	scene_admin = shared_ptr<SceneManager>(new SceneManager(*world, ""/* root folder is scene folder */));
  }
    
  void TearDown() override {}
  
  shared_ptr<ResourceManager> resource_admin;
  shared_ptr<SettingsManager> config;
  shared_ptr<EventManager> event_admin;	
  shared_ptr<FontManager> font_admin;
  shared_ptr<SDLGraphicsManager> graphics_admin;
  shared_ptr<AudioManager> audio_admin;
  shared_ptr<SceneManager> scene_admin;
  shared_ptr<GameWorldData> world;
	
};

TEST_F(SDLGraphicsManagerTests, Initialize)
{
	EXPECT_TRUE(graphics_admin->Initialize());
}
