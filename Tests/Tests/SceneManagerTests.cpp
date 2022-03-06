#include "pch.h"
#include "common/static_config.h"
#include "events/EventManager.h"
#include "font/FontManager.h"
#include "graphic/sdl_graphics_manager.h"
#include "objects/GameObject.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"

using namespace std;
using namespace gamelib;

class SceneManagerTests : public testing::Test {
 protected:
  void SetUp() override
  {  	
  	config = shared_ptr<SettingsManager>(new SettingsManager());
	event_admin = shared_ptr<EventManager>(new EventManager(*config, Logger));		
	font_admin = shared_ptr<FontManager>(new FontManager());
	graphics_admin = shared_ptr<sdl_graphics_manager>(new sdl_graphics_manager(*event_admin, Logger));
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
  shared_ptr<sdl_graphics_manager> graphics_admin;
  shared_ptr<AudioManager> audio_admin;
  shared_ptr<SceneManager> scene_admin;
  shared_ptr<game_world_data> world;
  Logger Logger;
	
};

TEST_F(SceneManagerTests, Initialize)
{
	EXPECT_TRUE(scene_admin->initialize());
	EXPECT_EQ(event_admin->get_subscriptions()[event_type::LevelChangedEventType].size(), 1) << "Scene manager not automatically subscribed to LevelChangedEventType event";
	EXPECT_EQ(event_admin->get_subscriptions()[event_type::AddGameObjectToCurrentScene].size(), 1) << "Scene manager not automatically subscribed to AddGameObjectToCurrentScene event";
	EXPECT_EQ(event_admin->get_subscriptions().size(), 4) << "Expected only 3 subscriptions to be made initially, included subscription by graphics manager";
}

TEST_F(SceneManagerTests, get_scene_layers)
{
	scene_admin->initialize();
	scene_admin->start_scene(1);
	event_admin->process_all_events();
	
	auto layers = scene_admin->get_scene_layers();	
	auto layer = layers.front();
	auto game_object = layer->game_objects.front();
	
	EXPECT_EQ(layers.size(), 1) << "Expected one layer";
	EXPECT_STREQ(layer->name.c_str(), "player0" );
	EXPECT_EQ(layer->x, 0) << "X position of Should initially be set to 0";
	EXPECT_EQ(layer->y, 0) << "Y position of Should initially be set to 0";
	EXPECT_TRUE(layer->visible) << "Layer not visible";
	EXPECT_EQ(layer->zorder, 0) << "Z-order is wrong";
	EXPECT_EQ(layer->game_objects.size(), 1) << "Expected 1 game object in the layer";	
	EXPECT_STREQ(game_object.lock()->get_identifier().c_str(), "square") << "Wrong game object";
		
}

