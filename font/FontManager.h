#pragma once
#include <memory>
#include "font_resource.h"
#include "asset/asset.h"

namespace tinyxml2 {
	class XMLElement;
}

namespace gamelib
{
	// Managers a set of font resources
	class FontManager
	{
	public:	 		
		// Creates an audio Resource
		std::shared_ptr<Asset> create_asset(tinyxml2::XMLElement * assetXmlElement) const;
		static std::shared_ptr<font_resource> to_resource(const std::shared_ptr<Asset>& asset);
	};
}
