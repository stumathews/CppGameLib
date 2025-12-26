#include "FontManager.h"
#include <SDL_ttf.h>
#include "FontAsset.h"
#include <tinyxml2.h>
#include "common/Common.h"

using namespace std;

namespace gamelib
{
	shared_ptr<Asset> FontManager::CreateAsset(const tinyxml2::XMLElement* assetXmlElement)
	{
		int uuid;
		const char* type;
		const char* path;
		const char* name;
		int scene;

		assetXmlElement->QueryIntAttribute("uid", &uuid);
		assetXmlElement->QueryStringAttribute("type", &type);
		assetXmlElement->QueryStringAttribute("filename", &path);
		assetXmlElement->QueryStringAttribute("name", &name);
		assetXmlElement->QueryIntAttribute("scene", &scene);
		
		return std::make_shared<FontAsset>(uuid, name, path, type, scene);
	}

	shared_ptr<FontAsset> FontManager::ToFontAsset(const shared_ptr<Asset>& asset)
	{
		if (asset->AssetType != Asset::AssetType::Font)
		{
			// TODO: Fix
			THROW(97, "Cannot cast a generic asset that is not an font asset to an font asset", "Font Manager");
		}

		return AsAsset<FontAsset>(asset);
	}
	
	FontManager* FontManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new FontManager();
		}
		return instance;
	}

	FontManager::~FontManager()
	{
		instance = nullptr;
	}

	void FontManager::Unload()
	{
		TTF_Quit();
	}

	bool FontManager::Initialize()
	{
		// Setup the SDL Font extension/library
		if(TTF_Init() == -1)
		{
			const string message("Could not initialize TTF");
			LogMessage(message);
			return false;			
		}
		return true;
	}

	FontManager* FontManager::instance = nullptr;
}
