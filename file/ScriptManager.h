#pragma once
#include <memory>

namespace tinyxml2
{
	class XMLElement;
}

namespace gamelib
{
	class ScriptAsset;
	class Asset;

	class ScriptManager
	{
	public:
		static ScriptManager* Get();
		~ScriptManager();
		static void Unload();
		static bool Initialize();
		ScriptManager(const ScriptManager& other) = delete;
		ScriptManager(const ScriptManager&& other) = delete;
		ScriptManager& operator=(const ScriptManager& other) = delete;
		ScriptManager& operator=(const ScriptManager&& other) = delete;

		static std::shared_ptr<Asset> CreateAsset(const tinyxml2::XMLElement* assetXmlElement);
		static std::shared_ptr<ScriptAsset> ToScriptAsset(const std::shared_ptr<Asset>& asset);
	protected:
		static ScriptManager* instance;
	private:
		ScriptManager() = default;

	};
}
