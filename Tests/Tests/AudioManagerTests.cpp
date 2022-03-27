#include "pch.h"

#include "audio/AudioManager.h"
#include "resource/ResourceManager.h"
#include "objects/GameObject.h"

using namespace std;
using namespace gamelib;

//int GameObject::ids = 0;

class AudioManagerTests : public testing::Test 
{
 protected:
  void SetUp() override
  {  	
  
    const shared_ptr<AudioAsset> ptr(new AudioAsset(0, RESOURCE_NAME, RESOURCE_PATH, RESOURCE_TYPE, RESOURCE_SCENE, *ResourceManager::Get()));
  	
  	audio_asset_ptr = dynamic_pointer_cast<Asset>(ptr);
  }
    
  //void TearDown() override {}

	
  shared_ptr<AudioManager> audio_admin;
  Asset *audio_asset = nullptr;
  shared_ptr<Asset> audio_asset_ptr;
  const string RESOURCE_NAME = "dymmy_audio_resource";
  const string RESOURCE_PATH = "no_real_path";
  const string RESOURCE_TYPE = "fx";
  const int RESOURCE_SCENE = 0;
	
};

TEST_F(AudioManagerTests, to_resource)
{
	// When casting a Asset* to a AudioAsset...
	auto resource = audio_admin->ToAudioAsset(audio_asset_ptr);
	
	EXPECT_STREQ(resource->name.c_str(), RESOURCE_NAME.c_str()) << "Resource name is invalid";
	EXPECT_EQ(resource->isLoadedInMemory, false) << "is loaded flag is invalid";
	EXPECT_STREQ(resource->path.c_str(), RESOURCE_PATH.c_str()) << "resource path is invalid";
	EXPECT_EQ(resource->scene, RESOURCE_SCENE) << "Resource scene is invalid";
}
