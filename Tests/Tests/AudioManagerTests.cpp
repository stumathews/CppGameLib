#include "pch.h"

#include <audio/AudioManager.h>
#include <resource/ResourceManager.h>
#include <objects/GameObject.h>

using namespace std;
using namespace gamelib;

class AudioManagerTests : public testing::Test 
{
 protected:
  void SetUp() override
  {
  	const shared_ptr<AudioAsset> ptr(new AudioAsset(0, resourceName, resourcePath, resourceType, resourceScene, *ResourceManager::Get()));
  	audioAssetPtr = dynamic_pointer_cast<Asset>(ptr);
  }
    
  //void TearDown() override {}

	
  shared_ptr<AudioManager> audioAdmin;
  Asset *audioAsset = nullptr;
  shared_ptr<Asset> audioAssetPtr;
  const string resourceName = "dummy_audio_resource";
  const string resourcePath = "no_real_path";
  const string resourceType = "fx";
  const int resourceScene = 0;
	
};

TEST_F(AudioManagerTests, to_resource)
{
	// When casting a Asset* to a AudioAsset...
	const auto resource = audioAdmin->ToAudioAsset(audioAssetPtr);
	
	EXPECT_STREQ(resource->Name.c_str(), resourceName.c_str()) << "Resource name is invalid";
	EXPECT_EQ(resource->IsLoadedInMemory, false) << "is loaded flag is invalid";
	EXPECT_STREQ(resource->FilePath.c_str(), resourcePath.c_str()) << "resource path is invalid";
	EXPECT_EQ(resource->SceneId, resourceScene) << "Resource scene is invalid";
}
