#include "FontManager.h"
#include "tinyxml2.h"
#include "common/Common.h"

using namespace std;

namespace gamelib
{
	FontManager* FontManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new FontManager();
		}
		return Instance;
	}

	FontManager::~FontManager()
	{
		Instance = nullptr;
	}

	FontManager* FontManager::Instance = nullptr;

	shared_ptr<Asset> FontManager::CreateAsset(const tinyxml2::XMLElement* assetXmlElement) const
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

		// Read anything specific to audio in the element here...

		auto font = std::make_shared<FontAsset>(uuid, name, path, type, scene);

		return font;
	}

	shared_ptr<FontAsset> FontManager::ToFontAsset(const shared_ptr<Asset>& asset)
	{
		if (asset->AssetType != Asset::AssetType::Font)
		{
			THROW(97, "Cannot cast a generic asset that is not an font asset to an font asset", "Font Manager")
		}

		return AsAsset<FontAsset>(asset);
	}
}
