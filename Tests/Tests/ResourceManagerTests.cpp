#include "pch.h"

#include "common/global_config.h"
#include "events/event_manager.h"
#include "font/font_manager.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/resource_manager.h"

using namespace std;
using namespace gamelib;

class ResourceManager : public testing::Test {
 protected:
  void SetUp() override
  {  	
  	config = make_shared<global_config>();
	event_admin = make_shared<event_manager>(config);		
	font_admin = make_shared<font_manager>();
	graphics_admin = make_shared<sdl_graphics_manager>(event_admin);
  	audio_admin = make_shared<audio_manager>();
	resource_admin = make_shared<resource_manager>(config, graphics_admin, font_admin, audio_admin);

  }
    
  //void TearDown() override {}
  const int exp_uid = 1;
  const string exp_name = "MainTheme.wav";
  const int exp_scene = 0;
  const string exp_type = "music";
  const string exp_filename = "Assets/Music/MainTheme.wav";
  const string resource_file_path = "Resources.xml";
  
  shared_ptr<resource_manager> resource_admin;
  shared_ptr<global_config> config;
  shared_ptr<event_manager> event_admin;	
  shared_ptr<font_manager> font_admin;
  shared_ptr<sdl_graphics_manager> graphics_admin;
  shared_ptr<audio_manager> audio_admin;

  void test_asset_against_baseline(shared_ptr<asset> asset)
  {
	EXPECT_EQ(asset->uid, exp_uid) << "uid is incorrect";
	EXPECT_EQ(asset->scene, exp_scene) << "Scene was incorrect";
	EXPECT_STREQ(asset->name.c_str(), exp_name.c_str()) << "Name was incorrect";	
	EXPECT_STREQ(asset->type.c_str(), exp_type.c_str()) << "Type was incorrect";
	EXPECT_STREQ(asset->path.c_str(), exp_filename.c_str()) << "Path was was incorrect";
  }
	
};

TEST_F(ResourceManager, Initialize)
{
	EXPECT_TRUE(resource_admin->initialize(event_admin)) << "Expected resource manager initialization to succeed";
}

TEST_F(ResourceManager, read_resources)
{
	resource_admin->read_resources(resource_file_path);
	EXPECT_EQ(resource_admin->get_resource_count(), 12) << "Expected 12 assets to be loaded";
}

TEST_F(ResourceManager, get_string)
{
	resource_admin->read_resources(resource_file_path);

	// When fetching an asset
	const auto asset = resource_admin->get(exp_name);

	// Ensure the asset is populated correctly
	test_asset_against_baseline(asset);
}
TEST_F(ResourceManager, get_int)
{
	resource_admin->read_resources(resource_file_path);

	// When fetching an asset
	const auto asset = resource_admin->get(exp_uid);

	// Ensure the asset is populated correctly
	test_asset_against_baseline(asset);
}


TEST_F(ResourceManager, unload)
{
	resource_admin->initialize(event_admin);
	resource_admin->read_resources(resource_file_path);
	resource_admin->unload();
	EXPECT_EQ(0, resource_admin->get_resource_unloaded_count()) << "Asset count is not 0 after unload";
}