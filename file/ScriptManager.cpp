#include "ScriptManager.h"

#include "asset/ScriptAsset.h"
#include "common/Common.h"
#include "exceptions/EngineException.h"
#include "file/tinyxml2.h"

namespace gamelib
{
	ScriptManager* ScriptManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new ScriptManager();
		}
		return instance;
	}

	ScriptManager::~ScriptManager() = default;

	void ScriptManager::Unload()
	{
	}

	bool ScriptManager::Initialize()
	{
		return true;// we don't have anything to initialize yet
	}

	std::shared_ptr<Asset> ScriptManager::CreateAsset(const tinyxml2::XMLElement* assetXmlElement)
	{
		int uuid = -1;
		const char* type = nullptr;
		const char* path = nullptr;
		const char* name = nullptr;
		int scene = -1;

		assetXmlElement->QueryIntAttribute("uid", &uuid);
		assetXmlElement->QueryStringAttribute("type", &type);
		assetXmlElement->QueryStringAttribute("filename", &path);
		assetXmlElement->QueryStringAttribute("name", &name);
		assetXmlElement->QueryIntAttribute("scene", &scene);

		if (uuid == -1 || type == nullptr || path == nullptr || name == nullptr || scene == -1)
		{
			THROW(99, "Error parsing script xml element", "ScriptManager");
		}

		return std::make_shared<ScriptAsset>(uuid, name, path, type, scene);
	}

	std::shared_ptr<ScriptAsset> ScriptManager::ToScriptAsset(const std::shared_ptr<Asset>& asset)
	{
		if (asset->AssetType != Asset::AssetType::Script)
		{
			// TODO: Fix
			THROW(97, "Cannot cast a generic asset that is not an font asset to an font asset", "Font Manager");
		}

		return AsAsset<ScriptAsset>(asset);
	}

	ScriptManager* ScriptManager::instance = nullptr;
}
