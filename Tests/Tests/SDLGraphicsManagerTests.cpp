#include "pch.h"

#include "common/static_config.h"
#include "events/EventManager.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "objects/GameObject.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"

using namespace std;
using namespace gamelib;

class SDLGraphicsManagerTests : public testing::Test {
 protected:
  void SetUp() override
  {  	
  	config = shared_ptr<SettingsManager>(new SettingsManager());

	event_admin = shared_ptr<EventManager>(new EventManager(*config, Logger));		
	font_admin = shared_ptr<FontManager>(new FontManager());
	graphics_admin = shared_ptr<SDLGraphicsManager>(new SDLGraphicsManager(*event_admin, Logger));
  	audio_admin = shared_ptr<AudioManager>(new AudioManager());
	resource_admin = shared_ptr<ResourceManager>(new ResourceManager(*config, *graphics_admin, *font_admin, *audio_admin, Logger));
	world = shared_ptr<game_world_data>(new game_world_data());
  	scene_admin = shared_ptr<SceneManager>(new SceneManager(*event_admin, *config, *resource_admin, *world, Logger, ""/* root folder is scene folder */));
  }
    
  //void TearDown() override {}
  
  shared_ptr<ResourceManager> resource_admin;
  shared_ptr<SettingsManager> config;
  shared_ptr<EventManager> event_admin;	
  shared_ptr<FontManager> font_admin;
  shared_ptr<SDLGraphicsManager> graphics_admin;
  shared_ptr<AudioManager> audio_admin;
  shared_ptr<SceneManager> scene_admin;
  shared_ptr<game_world_data> world;
  Logger Logger;
	
};

TEST_F(SDLGraphicsManagerTests, Initialize)
{
	EXPECT_TRUE(graphics_admin->initialize());
}
