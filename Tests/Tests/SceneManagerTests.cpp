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

class SceneManagerTests : public testing::Test 
{
 protected:

  void SetUp() override
  {  	
  	config = shared_ptr<SettingsManager>(new SettingsManager());
	event_admin = shared_ptr<EventManager>(new EventManager(*config, Logger));		
	font_admin = shared_ptr<FontManager>(new FontManager());
	graphics_admin = shared_ptr<SDLGraphicsManager>(new SDLGraphicsManager(*event_admin, Logger));
  	audio_admin = shared_ptr<AudioManager>(new AudioManager());
	resource_admin = shared_ptr<ResourceManager>(new ResourceManager(*config, *graphics_admin, *font_admin, *audio_admin, Logger));
	world = shared_ptr<GameWorldData>(new GameWorldData());
  	scene_admin = shared_ptr<SceneManager>(new SceneManager(*event_admin, *config, *resource_admin, *world, Logger, ""/* root folder is scene folder */));
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
  Logger Logger;
	
};

TEST_F(SceneManagerTests, Initialize)
{
	EXPECT_TRUE(scene_admin->Initialize());
	EXPECT_EQ(event_admin->GetSubscriptions()[EventType::LevelChangedEventType].size(), 1) << "Scene manager not automatically subscribed to LevelChangedEventType event";
	EXPECT_EQ(event_admin->GetSubscriptions()[EventType::AddGameObjectToCurrentScene].size(), 1) << "Scene manager not automatically subscribed to AddGameObjectToCurrentScene event";
	EXPECT_EQ(event_admin->GetSubscriptions().size(), 4) << "Expected only 3 subscriptions to be made initially, included subscription by graphics manager";
}

TEST_F(SceneManagerTests, get_scene_layers)
{
	resource_admin->Initialize(*event_admin);
	resource_admin->IndexResources("Resources.xml");
	scene_admin->Initialize();
	scene_admin->StartScene(1);
	event_admin->ProcessAllEvents();
	
	auto layers = scene_admin->GetLayers();	
	auto layer = layers.front();
	auto game_object = layer.layerObjects.front();
	
	EXPECT_EQ(layers.size(), 1) << "Expected one layer";
	EXPECT_STREQ(layer.name.c_str(), "player0" );
	EXPECT_EQ(layer.x, 0) << "X position of Should initially be set to 0";
	EXPECT_EQ(layer.y, 0) << "Y position of Should initially be set to 0";
	EXPECT_TRUE(layer.visible) << "Layer not visible";
	EXPECT_EQ(layer.zorder, 0) << "Z-order is wrong";
	EXPECT_EQ(layer.layerObjects.size(), 1) << "Expected 1 game object in the layer";	
	EXPECT_STREQ(game_object.lock()->GetName().c_str(), "AnimatedSprite") << "Wrong game object";
		
}

