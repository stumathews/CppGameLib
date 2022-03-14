#pragma once
#include <memory>
#include "FontAsset.h"
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
		std::shared_ptr<Asset> CreateAsset(tinyxml2::XMLElement * assetXmlElement) const;
		static std::shared_ptr<FontAsset> ToFontAsset(const std::shared_ptr<Asset>& asset);
	};
}
