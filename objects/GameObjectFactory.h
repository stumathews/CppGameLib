#pragma once
#include <memory>
#include "tinyxml2.h"
#include "GameObject.h"
#include "common/aliases.h"
#include "asset/Asset.h"
#include <objects/AnimatedSprite.h>

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
		/// <param name="scene_object_xml">Raw Object XML element</param>
		/// <returns>GameObject</returns>
		std::shared_ptr<GameObject> BuildGameObject(const tinyxml2::XMLElement* scene_object_xml) const;

		[[nodiscard]] std::shared_ptr<AnimatedSprite> BuildGraphic(const std::string& name, const std::string& type, std::shared_ptr<
			                                                           Asset> asset, const Coordinate<int>& position, bool IsVisible) const;
		[[nodiscard]] std::shared_ptr<AnimatedSprite> BuildSprite(const std::string& name, const std::string& type, std::shared_ptr<
			                                                          Asset> asset, const Coordinate<int>& position, bool IsVisible) const;
		[[nodiscard]] std::shared_ptr<StaticSprite> BuildStaticSprite(const std::string& name, const std::string& type,
		                                                              const std::shared_ptr<Asset>& asset, const Coordinate<int>& position) const;

	private:
		static GameObjectFactory& instance;

		static void SetupCommonSprite(const std::shared_ptr<AnimatedSprite>& _sprite, const std::shared_ptr<Asset>& asset, const std::shared_ptr<
			                              GraphicAsset>& graphicAsset, bool IsVisible);

		// Parsing object attribute handlers
		void OnBlueParse(uint& blue, const std::string& detailValue) const;
		void OnGreenParse(uint& green, std::string& detail_value) const;
		void OnRedParse(uint& red, std::string& detail_value) const;
		static void OnColourKeyParse(bool& color_key_enabled, std::string& detail_value);
		void OnPosYParse(uint& y, std::string& detail_value) const;
		static void OnVisibleParse(bool& visible, std::string& detail_value);
		void OnPosXParse(uint& x, std::string& detail_value) const;
		static void OnNameParse(std::string& x, std::string& detail_value);
		static void OnTypeParse(std::string& x, std::string& detail_value);
		static void GetAssetForResourceIdParse(std::string& detail_value, std::shared_ptr<Asset>& resource);

		[[nodiscard]] std::shared_ptr<GameObject> InitializeGameObject(const std::string& name, const std::string& type, Coordinate<int> position, bool IsVisible,
		                                                               const std::shared_ptr<Asset>& asset) const;

		// Private constructor 
		GameObjectFactory() = default;

		// Error handling
		static void ThrowCouldNotFindAssetException(std::shared_ptr<Asset>& asset, std::string& detail_value);
	};
}