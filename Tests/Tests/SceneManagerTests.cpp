#include "pch.h"
#include "common/static_config.h"
#include "events/event_manager.h"
#include "font/font_manager.h"
#include "graphic/sdl_graphics_manager.h"
#include "objects/GameObject.h"
#include "resource/resource_manager.h"
#include "scene/scene_manager.h"

using namespace std;
using namespace gamelib;

class SceneManager : public testing::Test {
 protected:
  void SetUp() override
  {  	
  	config = make_shared<settings_manager>();
	event_admin = make_shared<event_manager>(config);		
	font_admin = make_shared<font_manager>();
	graphics_admin = make_shared<sdl_graphics_manager>(event_admin);
  	audio_admin = make_shared<audio_manager>();
	resource_admin = make_shared<resource_manager>(config, graphics_admin, font_admin, audio_admin);
  	scene_admin = make_shared<scene_manager>(event_admin, config, resource_admin, ""/* root folder is scene folder */);
  }
    
  //void TearDown() override {}
  
  shared_ptr<resource_manager> resource_admin;
  shared_ptr<settings_manager> config;
  shared_ptr<event_manager> event_admin;	
  shared_ptr<font_manager> font_admin;
  shared_ptr<sdl_graphics_manager> graphics_admin;
  shared_ptr<audio_manager> audio_admin;
  shared_ptr<scene_manager> scene_admin;
	
};

TEST_F(SceneManager, Initialize)
{
	EXPECT_TRUE(scene_admin->initialize());
	EXPECT_EQ(event_admin->get_subscriptions()[event_type::LevelChangedEventType].size(), 1) << "Scene manager not automatically subscribed to LevelChangedEventType event";
	EXPECT_EQ(event_admin->get_subscriptions()[event_type::AddGameObjectToCurrentScene].size(), 1) << "Scene manager not automatically subscribed to AddGameObjectToCurrentScene event";
	EXPECT_EQ(event_admin->get_subscriptions().size(), 3) << "Expected only 3 subscriptions to be made initially, included subscription by graphics manager";
}

TEST_F(SceneManager, get_scene_layers)
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
	EXPECT_STREQ(game_object->get_identifier().c_str(), "square") << "Wrong game object";
		
}

