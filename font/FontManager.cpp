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

	shared_ptr<Asset> FontManager::CreateAsset(tinyxml2::XMLElement* element) const
	{
		int uuid;
		const char* type;
		const char* path;
		const char* name;
		int scene;

		element->QueryIntAttribute("uid", &uuid);
		element->QueryStringAttribute("type", &type);
		element->QueryStringAttribute("filename", &path);
		element->QueryStringAttribute("name", &name);
		element->QueryIntAttribute("scene", &scene);

		// Read anything specific to audio in the element here...

		auto font = shared_ptr<FontAsset>(new FontAsset(uuid, name, path, type, scene));

		return font;
	}

	shared_ptr<FontAsset> FontManager::ToFontAsset(const shared_ptr<Asset>& asset)
	{
		if (asset->assetType != Asset::AssetType::Font)
		{
			THROW(97, "Cannot cast a generic asset that is not an font asset to an font asset", "Font Manager");
		}

		return AsAsset<FontAsset>(asset);
	}
}
