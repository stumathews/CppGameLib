#pragma once
#include <memory>
#include "file/tinyxml2.h"
#include "GameObject.h"
#include "common/aliases.h"
#include "asset/Asset.h"
#include <character/AnimatedSprite.h>

namespace gamelib
{
	class ResourceManager;
	class StaticSprite;

	/// <summary>
	/// Constructs a Game Object from the details about a game object (usually from file)
	/// </summary>
	class GameObjectFactory
	{
	public:	
		GameObjectFactory(GameObjectFactory const&) = delete;	
		GameObjectFactory& operator=(GameObjectFactory const&)  = delete;
		~GameObjectFactory() = default;		

		static GameObjectFactory& Get();

		/// <summary>
		/// Build a game object
		/// </summary>
		/// <param name="sceneObjectXml">Raw Object XML element</param>
		/// <returns>GameObject</returns>
		std::shared_ptr<GameObject> BuildGameObject(const tinyxml2::XMLElement* sceneObjectXml) const;

		[[nodiscard]] std::shared_ptr<StaticSprite> BuildGraphic(
			const std::shared_ptr<Asset>& asset, const Coordinate<int>& position) const;
		[[nodiscard]] static std::shared_ptr<AnimatedSprite> BuildSprite(const std::string& name, const std::string& type,
		                                                          const std::shared_ptr<
			                                                          Asset>& asset, const Coordinate<int>& position, bool isVisible);
		[[nodiscard]] std::shared_ptr<StaticSprite> BuildStaticSprite(
			const std::shared_ptr<Asset>& asset, const Coordinate<int>& position) const;

	private:
		static GameObjectFactory& instance;

		static void SetupCommonSprite(const std::shared_ptr<AnimatedSprite>& sprite, const std::shared_ptr<Asset>& asset, const std::shared_ptr<
			                              GraphicAsset>& graphicAsset, bool isVisible);

		// Parsing object attribute handlers
		void OnBlueParse(uint& blue, const std::string& detailValue) const;
		void OnGreenParse(uint& green, const std::string& detailValue) const;
		void OnRedParse(uint& red, const std::string& detailValue) const;
		static void OnColourKeyParse(bool& color_key_enabled, const std::string& detailValue);
		void OnPosYParse(uint& y, const std::string& detailValue) const;
		static void OnVisibleParse(bool& visible, const std::string& detailValue);
		void OnPosXParse(uint& x, const std::string& detailValue) const;
		static void OnNameParse(std::string& x, const std::string& detailValue);
		static void OnTypeParse(std::string& x, const std::string& detailValue);
		static void GetAssetForResourceIdParse(const std::string& detailValue, std::shared_ptr<Asset>& resource);

		[[nodiscard]] std::shared_ptr<GameObject> InitializeGameObject(const std::string& name, const std::string& type, Coordinate<int> position, bool IsVisible,
		                                                               const std::shared_ptr<Asset>& asset) const;

		// Private constructor 
		GameObjectFactory() = default;

		// Error handling
		static void ThrowCouldNotFindAssetException(const std::shared_ptr<Asset>& asset, const std::string& detailValue);
	};
}