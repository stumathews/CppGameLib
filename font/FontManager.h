#pragma once
#include <memory>

#include "asset/asset.h"

namespace tinyxml2 {
	// ReSharper disable once CppInconsistentNaming
	class XMLElement;
}

namespace gamelib
{
	class FontAsset;
	class FontManager
	{
	public:	 		
		static FontManager* Get();
		~FontManager();
		static void Unload();
		static bool Initialize();
		FontManager(const FontManager& other) = delete;
		FontManager(const FontManager&& other) = delete;
		FontManager& operator=(const FontManager& other) = delete;
		FontManager& operator=(const FontManager&& other) = delete;

		static std::shared_ptr<Asset> CreateAsset(const tinyxml2::XMLElement * assetXmlElement);
		static std::shared_ptr<FontAsset> ToFontAsset(const std::shared_ptr<Asset>& asset);
	protected:
		static FontManager* instance;
	private:
		FontManager() = default;

	};
}
