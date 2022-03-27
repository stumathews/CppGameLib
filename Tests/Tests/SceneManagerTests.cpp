﻿#include "pch.h"
#include "common/StaticConfig.h"
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
	  EventManager::Get()->ClearSubscribers();
  }
    
  void TearDown() override {}

	
};

TEST_F(SceneManagerTests, Initialize)
{
	GameWorldData data;
	SceneManager sceneManager = SceneManager(data);
	EXPECT_TRUE(sceneManager.Initialize());
	EXPECT_EQ(EventManager::Get()->GetSubscriptions()[EventType::LevelChangedEventType].size(), 1) << "Scene manager not automatically subscribed to LevelChangedEventType event";
	EXPECT_EQ(EventManager::Get()->GetSubscriptions()[EventType::AddGameObjectToCurrentScene].size(), 1) << "Scene manager not automatically subscribed to AddGameObjectToCurrentScene event";
	EXPECT_EQ(EventManager::Get()->GetSubscriptions().size(), 4) << "Expected only 3 subscriptions to be made initially, included subscription by graphics manager";
}

TEST_F(SceneManagerTests, get_scene_layers)
{
	ResourceManager::Get()->Initialize();
	ResourceManager::Get()->IndexResources("Resources.xml");
	GameWorldData data;
	SceneManager sceneManager = SceneManager(data, "");
	sceneManager.Initialize();
	sceneManager.StartScene(1);
	EventManager::Get()->ProcessAllEvents();
	
	auto layers = sceneManager.GetLayers();
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

