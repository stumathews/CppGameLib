#pragma once
#include <memory>
#include "FontAsset.h"
#include "asset/asset.h"

namespace tinyxml2 {
	// ReSharper disable once CppInconsistentNaming
	class XMLElement;
}

namespace gamelib
{
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

		std::shared_ptr<Asset> CreateAsset(const tinyxml2::XMLElement * assetXmlElement) const;
		static std::shared_ptr<FontAsset> ToFontAsset(const std::shared_ptr<Asset>& asset);
	protected:
		static FontManager* instance;
	private:
		FontManager() = default;

	};
}
