#include "pch.h"

#include <gtest/gtest.h>

#include "events/EventManager.h"
#include "events/SceneChangedEvent.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"

using namespace std;
namespace gamelib
{

	class ResourceManagerTests : public testing::Test
	{
	 protected:

	  void SetUp() override
	  { 
	  }
	    
	  void TearDown() override {}


	  const int exp_uid = 1;
	  const string exp_name = "LevelMusic4";
	  const int exp_scene = 4;
	  const string exp_type = "music";
	  const string exp_filename = "Assets/Music/MainTheme.wav";
	  const string resource_file_path = "Resources.xml";
	   

	  void test_asset_against_baseline(shared_ptr<Asset> asset) const
	  {
		EXPECT_EQ(asset->Uid, exp_uid) << "uid is incorrect";
		EXPECT_EQ(asset->SceneId, exp_scene) << "Scene was incorrect";
		EXPECT_STREQ(asset->Name.c_str(), exp_name.c_str()) << "Name was incorrect";	
		EXPECT_STREQ(asset->Type.c_str(), exp_type.c_str()) << "Type was incorrect";
		EXPECT_STREQ(asset->FilePath.c_str(), exp_filename.c_str()) << "Path was was incorrect";
	  }
		
	};

	TEST_F(ResourceManagerTests, Initialize)
	{
		EXPECT_TRUE(ResourceManager::Get()->Initialize("Resources.xml")) << "Expected resource manager initialization to succeed";
		EXPECT_EQ(EventManager::Get()->GetSubscriptions()[LevelChangedEventTypeEventId].size(), 1) << "Expected to subscribe to LevelChangedEventType";
		EXPECT_STREQ(
			EventManager::Get()->GetSubscriptions()[LevelChangedEventTypeEventId][0]->GetSubscriberName().c_str(),
			ResourceManager::Get()->GetSubscriberName().c_str()) << "Unexpected subscriber";
	}

	TEST_F(ResourceManagerTests, read_resources)
	{
		ResourceManager::Get()->IndexResourceFile(resource_file_path);
		EXPECT_EQ(ResourceManager::Get()->GetCountResources(), 18) << "Expected 18 assets to be loaded";
	}

	TEST_F(ResourceManagerTests, get_resource_via_string)
	{
		ResourceManager::Get()->IndexResourceFile(resource_file_path);

		// When fetching an asset using string identifier
		const auto asset = ResourceManager::Get()->GetAssetInfo(exp_name);

		// Ensure the asset is populated correctly
		test_asset_against_baseline(asset);
	}
	TEST_F(ResourceManagerTests, get_resource_via_int)
	{
		ResourceManager::Get()->IndexResourceFile(resource_file_path);

		// When fetching an asset using integer uid
		const auto asset = ResourceManager::Get()->GetAssetInfo(exp_uid);
		
		// Ensure the asset is populated correctly
		test_asset_against_baseline(asset);
	}


	TEST_F(ResourceManagerTests, unload)
	{
		ResourceManager::Get()->Initialize("Resources.xml");
		ResourceManager::Get()->IndexResourceFile(resource_file_path);
		ResourceManager::Get()->Unload();
		EXPECT_EQ(0, ResourceManager::Get()->GetCountUnloadedResources()) << "Asset count is not 0 after unload";
	}
}
