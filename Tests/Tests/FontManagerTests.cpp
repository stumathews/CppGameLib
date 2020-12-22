#include "pch.h"

#include "common/global_config.h"
#include "events/event_manager.h"
#include "font/font_manager.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/resource_manager.h"


using namespace std;
using namespace gamelib;

class FontManager : public testing::Test {
 protected:
  void SetUp() override
  {  	
  	config = make_shared<global_config>();
	event_admin = make_shared<event_manager>(config);		
	font_admin = make_shared<font_manager>();
	graphics_admin = make_shared<sdl_graphics_manager>(event_admin);
  	audio_admin = make_shared<audio_manager>();
	resource_admin = make_shared<resource_manager>(config, graphics_admin, font_admin, audio_admin);

  	 const shared_ptr<font_resource> ptr(new font_resource(0, RESOURCE_NAME, RESOURCE_PATH, RESOURCE_TYPE, RESOURCE_SCENE));
  	
  	font_asset_ptr = dynamic_pointer_cast<asset>(ptr);
  }
    
  //void TearDown() override {}
  
  shared_ptr<resource_manager> resource_admin;
  shared_ptr<global_config> config;
  shared_ptr<event_manager> event_admin;	
  shared_ptr<font_manager> font_admin;
  shared_ptr<sdl_graphics_manager> graphics_admin;
  shared_ptr<audio_manager> audio_admin;

  const string RESOURCE_NAME = "arial.ttf";
  const string RESOURCE_PATH = "Assets/fonts/arial.ttf";
  const string RESOURCE_TYPE = "font";
  const int RESOURCE_SCENE = 0;
	
  asset *audio_asset = nullptr;
  shared_ptr<asset> font_asset_ptr;
	
};

TEST_F(FontManager, to_resource)
{
	// When casting a asset* to a font_resource...
	auto resource = font_admin->to_resource(font_asset_ptr);
	
	EXPECT_STREQ(resource->name.c_str(), RESOURCE_NAME.c_str()) << "Resource name is invalid";
	EXPECT_EQ(resource->is_loaded, false) << "is loaded flag is invalid";
	EXPECT_STREQ(resource->path.c_str(), RESOURCE_PATH.c_str()) << "resource path is invalid";
	EXPECT_EQ(resource->scene, RESOURCE_SCENE) << "Resource scene is invalid";
}

