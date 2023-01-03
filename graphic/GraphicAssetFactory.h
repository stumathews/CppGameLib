#pragma once
#include "GraphicAsset.h"
#include "tinyxml2.h"
#include <memory>
#include "SpriteAsset.h"

namespace gamelib
{
	class GraphicAssetFactory
	{
	public:
		static GraphicAssetFactory* Get();
		GraphicAssetFactory(const GraphicAssetFactory& other) = delete;
		GraphicAssetFactory& operator=(const GraphicAssetFactory& other) = delete;
		~GraphicAssetFactory();

		/// <summary>
		/// Parse a Graphic Asset
		/// </summary>
		/// <param name="resourceElement">Asset XML</param>
		/// <returns>Pointer to a Graphic Asset</returns>
		std::shared_ptr<Asset> Parse(tinyxml2::XMLElement* resourceElement) const;
		void ParseSpriteAnimation(tinyxml2::XMLNode* pAssetChild, const std::shared_ptr<SpriteAsset>& sprite) const;
		void ParseSprite(tinyxml2::XMLNode* pAssetChild, const std::shared_ptr<SpriteAsset>& sprite) const;
		void ParseSpriteKeyFrames(tinyxml2::XMLNode* pKeyFrames, const std::shared_ptr<SpriteAsset>& sprite) const;
		void ParseSpriteKeyFrame(tinyxml2::XMLNode* pKeyFrame, const std::shared_ptr<SpriteAsset>& sprite) const;
		static std::map<std::string, std::string> GetNodeAttributes(tinyxml2::XMLNode* pAssetNode);
	protected:
		static GraphicAssetFactory* Instance;
	private:
		GraphicAssetFactory() = default;
	};
}

