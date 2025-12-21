
#include <lua.h>
#include <gtest/gtest.h>

#include "asset/ScriptAsset.h"
#include "file/ScriptManager.h"
#include "file/SerializationManager.h"
#include "resource/ResourceManager.h"

using namespace std;
namespace gamelib
{
	class ScriptManagerTests: public testing::Test
	{
	 protected:
		

		 void SetUp() override
		 {		 
		 }

		 void TearDown() override 
		 {
		 }

	};

	TEST_F(ScriptManagerTests, ScriptAsset)
	{
		{
			// Simulate a script asset, i.e, the meta-data about the script
			ScriptAsset myScript(1, "TestScript", "TestScript.lua", "script", 0);

			// By default, it should not be loaded into memory
			EXPECT_FALSE(myScript.IsLoadedInMemory);

			// Load the script into memory
			myScript.Load();

			// Now it should be loaded
			EXPECT_TRUE(myScript.IsLoadedInMemory);

			// Get the script content
			auto script = myScript.GetScriptContent();

			// Should contain the script content
			EXPECT_NE(script, nullptr) << "Expected script content to be loaded";

			// Unload the script from memory
			myScript.Unload();

			// Now it should not be loaded
			EXPECT_FALSE(myScript.IsLoadedInMemory);

			// Get the script content
			script = myScript.GetScriptContent();

			// Should not contain the script content
			EXPECT_EQ(script, nullptr) << "Expected script content to be loaded";
		}
	}

	TEST_F(ScriptManagerTests, ResourceManagerWorks)
	{
		// Index the resources file
		ResourceManager::Get()->IndexResourceFile("Resources.xml");

		// When fetching an asset using string identifier
		const auto asset = ResourceManager::Get()->GetAssetInfo("TestScript");

		// Should find the asset
		EXPECT_NE(asset, nullptr) << "Expected to find TestScript asset";

		// Cast to a script asset
		const auto scriptAsset = ScriptManager::ToScriptAsset(asset);

		// Should be a script asset
		EXPECT_NE(scriptAsset, nullptr) << "Expected to cast to a script asset";
		EXPECT_EQ(scriptAsset->Uid, 10) << "uid is incorrect";
		EXPECT_EQ(scriptAsset->SceneId, 0) << "Scene was incorrect";
		EXPECT_STREQ(scriptAsset->Name.c_str(), "TestScript") << "Name was incorrect";
		EXPECT_STREQ(scriptAsset->Type.c_str(), "script") << "Type was incorrect";
		EXPECT_STREQ(scriptAsset->FilePath.c_str(), "TestScript.lua") << "Path was was incorrect";

		// By default, it should not be loaded into memory
		EXPECT_FALSE(scriptAsset->IsLoadedInMemory);

		// Load the script into memory
		scriptAsset->Load();

		// Get the script content
		auto script = scriptAsset->GetScriptContent();

		// Should contain the script content
		EXPECT_NE(script, nullptr) << "Expected script content to be loaded";

		// Now unload it
		scriptAsset->Unload();

		// Get script content
		script = scriptAsset->GetScriptContent();

		// Should not contain the script content
		EXPECT_EQ(script, nullptr) << "Expected script content to be unloaded";
	}

}
