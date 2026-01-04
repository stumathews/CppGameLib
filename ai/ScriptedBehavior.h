//
// Created by stuart on 03/01/2026.
//

#ifndef CPPGAMELIB_SCRIPTEDBEHAVIOR_H
#define CPPGAMELIB_SCRIPTEDBEHAVIOR_H
#include "file/Logger.h"
#include "time/PeriodicTimer.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <memory>

#include "Behavior.h"
#include "asset/ScriptAsset.h"
#include "file/ScriptManager.h"
#include "resource/ResourceManager.h"


namespace gamelib
{
    class ScriptAsset;
}

class ScriptedBehavior : gamelib::Behavior
{
public:
    explicit ScriptedBehavior(std::string resourceName, unsigned long runEveryMs = 0) : scriptName(std::move(resourceName))
    {
        // We will run our logic at a periodic frequency
        logicTimer.SetFrequency(static_cast<int>(runEveryMs));
    }

    void OnInitialize() override
    {
        // Fetch script asset from resource manager
        const auto asset = gamelib::ResourceManager::Get()->GetAssetInfo(scriptName);

        LoadAssetIfNotLoaded(asset);

        // Save it to a ScriptAsset
        scriptAsset = gamelib::ScriptManager::ToScriptAsset(asset);

        // Create a new Lua state for this script
        luaState = luaL_newstate();

        // Load Lua standard libraries
        luaL_openlibs(luaState);
    }

    void OnTerminate() override
    {
        // Close lua state
        lua_close(luaState);

        // Note: No need to unload asset it gets unloaded automatically
    }

    gamelib::Status Update(unsigned long deltaMs) override
    {
        logicTimer.Update(deltaMs);

        // We won't run this script on every frame, but will instead run it periodically to save resources
        logicTimer.DoIfReady([&]()
        {
            // Execute a Lua script
            luaL_dostring(luaState, scriptAsset->GetScriptContent()->c_str());
        });

        return gamelib::Status::Success;
    }
private:
    std::shared_ptr<gamelib::ScriptAsset> scriptAsset;
    lua_State* luaState = nullptr;
    std::string scriptName;
    gamelib::PeriodicTimer logicTimer;
    void LoadAssetIfNotLoaded(const std::shared_ptr<gamelib::Asset> &asset) const
    {
        if (!asset->IsLoadedInMemory)
        {
            // Normally this is handled automatically by the scene manager
            std::stringstream message;

            message << "Resource '" << scriptName << "' was not pre-loaded. Did you forget to use the SceneManager to load assets by scene/level?";
            gamelib::Logger::Get()->LogThis(message.str());

            // Manually load the asset
            asset->Load();
        }
    }

};

#endif //CPPGAMELIB_SCRIPTEDBEHAVIOR_H