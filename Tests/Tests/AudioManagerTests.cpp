#include "pch.h"

#include "audio/AudioManager.h"
#include "resource/ResourceManager.h"
#include "objects/GameObject.h"

using namespace std;
using namespace gamelib;

//int GameObject::ids = 0;

class AudioManagerTests : public testing::Test {
 protected:
  void SetUp() override
  {  	
  	const shared_ptr<SettingsManager> settings_admin(new SettingsManager);
	Logger gameLogger;
	const shared_ptr<EventManager> event_admin(new EventManager(*settings_admin, gameLogger));		
	const shared_ptr<FontManager> font_admin(new FontManager);		
	audio_admin = create_audio_manager();
	const shared_ptr<SDLGraphicsManager> graphics_admin(new SDLGraphicsManager(*event_admin, gameLogger));		
	const shared_ptr<ResourceManager> resource_admin(new ResourceManager(*settings_admin, *graphics_admin, *font_admin, *audio_admin, gameLogger));
    const shared_ptr<audio_resource> ptr(new audio_resource(0, RESOURCE_NAME, RESOURCE_PATH, RESOURCE_TYPE, RESOURCE_SCENE, *resource_admin));
  	
  	audio_asset_ptr = dynamic_pointer_cast<asset>(ptr);
  }
    
  //void TearDown() override {}
  static shared_ptr<gamelib::AudioManager> create_audio_manager()
  {
	shared_ptr<gamelib::AudioManager> audio_admin(new gamelib::AudioManager());
  	return audio_admin;
  }
	
  shared_ptr<AudioManager> audio_admin;
  asset *audio_asset = nullptr;
  shared_ptr<asset> audio_asset_ptr;
  const string RESOURCE_NAME = "dymmy_audio_resource";
  const string RESOURCE_PATH = "no_real_path";
  const string RESOURCE_TYPE = "fx";
  const int RESOURCE_SCENE = 0;
	
};

TEST_F(AudioManagerTests, to_resource)
{
	// When casting a asset* to a audio_resource...
	auto resource = audio_admin->to_resource(audio_asset_ptr);
	
	EXPECT_STREQ(resource->name.c_str(), RESOURCE_NAME.c_str()) << "Resource name is invalid";
	EXPECT_EQ(resource->is_loaded, false) << "is loaded flag is invalid";
	EXPECT_STREQ(resource->path.c_str(), RESOURCE_PATH.c_str()) << "resource path is invalid";
	EXPECT_EQ(resource->scene, RESOURCE_SCENE) << "Resource scene is invalid";
}
