
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <gtest/gtest.h>

#include "asset/ScriptAsset.h"
#include "file/ScriptManager.h"
#include "resource/ResourceManager.h"

using namespace std;
namespace gamelib
{
	class LuaTests: public testing::Test
	{
	 protected:
		

		 void SetUp() override
		 {		 
		 }

		 void TearDown() override 
		 {
		 }

	};

	TEST_F(LuaTests, Basic)
	{
		// Create a new Lua state
		lua_State* luaState = luaL_newstate(); 

		// Load Lua standard libraries
		luaL_openlibs(luaState);                 

		// Execute a simple Lua script
		luaL_dostring(luaState, "print('Hello from Lua!')");

		lua_close(luaState);
	}

	TEST_F(LuaTests, TestScriptLoadedFromFile)
	{
		const auto resourceManager = ResourceManager::Get();
		const auto scriptManager = ScriptManager::Get();

		// Create a new Lua state
		lua_State* luaState = luaL_newstate();

		// Load Lua standard libraries
		luaL_openlibs(luaState);

		// Index the resources file
		ResourceManager::Get()->IndexResourceFile("Resources.xml");

		// Get script asset details
		const auto asset = resourceManager->GetAssetInfo("TestScript");

		// Load the script into memory
		asset->Load();

		const auto scriptAsset = scriptManager->ToScriptAsset(asset);

		// Execute a simple Lua script
		luaL_dostring(luaState, scriptAsset->GetScriptContent()->c_str());

		lua_close(luaState);
	}

}
