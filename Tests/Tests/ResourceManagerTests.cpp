#include "pch.h"

#include "common/static_config.h"
#include "events/EventManager.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"

using namespace std;
using namespace gamelib;

class ResourceManagerTests : public testing::Test {
 protected:
  void SetUp() override
  {  	
  	config = shared_ptr<SettingsManager>(new SettingsManager());
	event_admin = shared_ptr<EventManager>(new EventManager(*config, Logger));		
	font_admin = shared_ptr<FontManager>(new FontManager());
	graphics_admin = shared_ptr<SDLGraphicsManager>(new SDLGraphicsManager(*event_admin, Logger));
  	audio_admin = shared_ptr<AudioManager>(new AudioManager());
	resource_admin = shared_ptr<ResourceManager>(new ResourceManager(*config, *graphics_admin, *font_admin, *audio_admin, Logger));

  }
    
  //void TearDown() override {}
  const int exp_uid = 1;
  const string exp_name = "MainTheme.wav";
  const int exp_scene = 0;
  const string exp_type = "music";
  const string exp_filename = "Assets/Music/MainTheme.wav";
  const string resource_file_path = "Resources.xml";
  
  shared_ptr<ResourceManager> resource_admin;
  shared_ptr<SettingsManager> config;
  shared_ptr<EventManager> event_admin;	
  shared_ptr<FontManager> font_admin;
  shared_ptr<SDLGraphicsManager> graphics_admin;
  shared_ptr<AudioManager> audio_admin;
  Logger Logger;

  void test_asset_against_baseline(shared_ptr<asset> asset) const
  {
	EXPECT_EQ(asset->uid, exp_uid) << "uid is incorrect";
	EXPECT_EQ(asset->scene, exp_scene) << "Scene was incorrect";
	EXPECT_STREQ(asset->name.c_str(), exp_name.c_str()) << "Name was incorrect";	
	EXPECT_STREQ(asset->type.c_str(), exp_type.c_str()) << "Type was incorrect";
	EXPECT_STREQ(asset->path.c_str(), exp_filename.c_str()) << "Path was was incorrect";
  }
	
};

TEST_F(ResourceManagerTests, Initialize)
{
	EXPECT_TRUE(resource_admin->initialize(*event_admin)) << "Expected resource manager initialization to succeed";
	EXPECT_EQ(event_admin->get_subscriptions()[event_type::LevelChangedEventType].size(), 1) << "Expected to subscribe to LevelChangedEventType";
	EXPECT_STREQ(
		event_admin->get_subscriptions()[event_type::LevelChangedEventType][0]->get_subscriber_name().c_str(),
		resource_admin->get_subscriber_name().c_str()) << "Unexpected subscriber";
}

TEST_F(ResourceManagerTests, read_resources)
{
	resource_admin->IndexResources(resource_file_path);
	EXPECT_EQ(resource_admin->get_resource_count(), 12) << "Expected 12 assets to be loaded";
}

TEST_F(ResourceManagerTests, get_resource_via_string)
{
	resource_admin->IndexResources(resource_file_path);

	// When fetching an asset using string identifier
	const auto asset = resource_admin->get(exp_name);

	// Ensure the asset is populated correctly
	test_asset_against_baseline(asset);
}
TEST_F(ResourceManagerTests, get_resource_via_int)
{
	resource_admin->IndexResources(resource_file_path);

	// When fetching an asset using integer uid
	const auto asset = resource_admin->get(exp_uid);
	
	// Ensure the asset is populated correctly
	test_asset_against_baseline(asset);
}


TEST_F(ResourceManagerTests, unload)
{
	resource_admin->initialize(*event_admin);
	resource_admin->IndexResources(resource_file_path);
	resource_admin->unload();
	EXPECT_EQ(0, resource_admin->get_resource_unloaded_count()) << "Asset count is not 0 after unload";
}
