#include "FontManager.h"
#include "tinyxml2.h"
#include "common/Common.h"

namespace gamelib
{
	std::shared_ptr<Asset> FontManager::create_asset(tinyxml2::XMLElement* element) const
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

		auto font = std::make_shared<font_resource>(uuid, name, path, type, scene);

		return font;
	}

	std::shared_ptr<font_resource> FontManager::to_resource(const std::shared_ptr<Asset>& asset)
	{
		return AsAsset<font_resource>(asset);
	}
}
