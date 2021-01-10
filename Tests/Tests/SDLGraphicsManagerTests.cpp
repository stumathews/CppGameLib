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

class SDLGraphicsManager : public testing::Test {
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

TEST_F(SDLGraphicsManager, Initialize)
{
	EXPECT_TRUE(graphics_admin->initialize());
}
