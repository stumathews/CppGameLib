﻿#include "pch.h"

#include "common/StaticConfig.h"
#include "events/EventManager.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"

using namespace std;
using namespace gamelib;

class FontManagerTests : public testing::Test 
{
 protected:

  void SetUp() override
  { 
  	const shared_ptr<FontAsset> ptr(new FontAsset(0, resource_name, resource_path, resource_type, resource_scene));  	
  	font_asset_ptr = dynamic_pointer_cast<Asset>(ptr);
  }
    
  void TearDown() override {}
   
  shared_ptr<FontManager> font_admin;

  const string resource_name = "arial.ttf";
  const string resource_path = "Assets/fonts/arial.ttf";
  const string resource_type = "font";
  const int resource_scene = 0;
	
  Asset *audio_asset = nullptr;
  shared_ptr<Asset> font_asset_ptr;	
};

TEST_F(FontManagerTests, to_resource)
{
	// When casting a Asset* to a FontAsset...
	auto resource = FontManager::Get()->ToFontAsset(font_asset_ptr);

	// Ensure that...
	EXPECT_STREQ(resource->name.c_str(), resource_name.c_str()) << "Resource name is invalid";
	EXPECT_EQ(resource->IsLoadedInMemory, false) << "is loaded flag is invalid";
	EXPECT_STREQ(resource->path.c_str(), resource_path.c_str()) << "resource path is invalid";
	EXPECT_EQ(resource->scene, resource_scene) << "Resource scene is invalid";
}

