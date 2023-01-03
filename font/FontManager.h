#pragma once
#include <memory>
#include "FontAsset.h"
#include "asset/asset.h"

namespace tinyxml2 {
	class XMLElement;
}

namespace gamelib
{
	class FontManager
	{
	public:	 		
		static FontManager* Get();
		~FontManager();
		std::shared_ptr<Asset> CreateAsset(const tinyxml2::XMLElement * assetXmlElement) const;
		static std::shared_ptr<FontAsset> ToFontAsset(const std::shared_ptr<Asset>& asset);
	protected:
		static FontManager* Instance;
	private:
		FontManager() = default;

	};
}
