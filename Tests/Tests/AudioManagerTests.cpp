#include "pch.h"

//#include "audio/AudioManager.h"
//#include "resource/resource_manager.h"

//using namespace std;
//using namespace gamelib;

//class AudioManager : public testing::Test {
// protected:
//  void SetUp() override
//  {  	
//  	const shared_ptr<global_config> global_config(new gamelib::global_config);
//	const shared_ptr<event_manager> event_admin(new event_manager(global_config));		
//	const shared_ptr<font_manager> font_admin(new font_manager);		
//	audio_admin = create_audio_manager();
//	const shared_ptr<sdl_graphics_manager> graphics_admin(new sdl_graphics_manager(event_admin));		
//	const shared_ptr<resource_manager> resource_admin(new resource_manager(global_config, graphics_admin, font_admin, audio_admin ));
//
//    const shared_ptr<audio_resource> ptr(new audio_resource(0, RESOURCE_NAME, "no_real_path", "fx", 0, resource_admin));
//  	
//  	audio_asset_ptr = dynamic_pointer_cast<asset>(ptr);
//  }
//    
//  //void TearDown() override {}
//  shared_ptr<audio_manager> create_audio_manager()
//  {
//	shared_ptr<audio_manager> audio_admin(new audio_manager);
//  	return audio_admin;
//  }
//  shared_ptr<audio_manager> audio_admin;
//  asset *audio_asset;
//	shared_ptr<asset> audio_asset_ptr;
//  const string RESOURCE_NAME = "dymmy_audio_resource";
//	
//};
//
//TEST_F(AudioManager, NullTest)
//{
//	auto resource = audio_admin->to_resource(audio_asset_ptr);
//	EXPECT_STREQ(resource->name.c_str(), RESOURCE_NAME.c_str());
//}
