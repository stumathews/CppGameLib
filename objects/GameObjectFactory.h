#pragma once
#include <memory>
#include "tinyxml2.h"
#include "GameObject.h"
#include "common/aliases.h"
#include "asset/Asset.h"
#include <graphic/ColourKey.h>
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
		std::shared_ptr<GameObject> BuildGameObject(tinyxml2::XMLElement* scene_object_xml) const;

		std::shared_ptr<gamelib::AnimatedSprite> BuildGraphic(std::string name, std::string type, std::shared_ptr<gamelib::Asset> asset, const gamelib::coordinate<int>& position, bool IsVisible) const;
		std::shared_ptr<gamelib::AnimatedSprite> BuildSprite(std::string name, std::string type, std::shared_ptr<gamelib::Asset> asset, const gamelib::coordinate<int>& position, bool IsVisible) const;
		std::shared_ptr<gamelib::StaticSprite> BuildStaticSprite(std::string name, std::string type, std::shared_ptr<gamelib::Asset> asset, const gamelib::coordinate<int>& position);
		
	private:
		static GameObjectFactory& instance;

		void SetupCommonSprite(std::shared_ptr<gamelib::AnimatedSprite>& _sprite, std::shared_ptr<gamelib::Asset>& asset, std::shared_ptr<gamelib::GraphicAsset>& graphicAsset, bool IsVisible) const;

		// Parsing object attribute handlers
		void OnBlueParse(gamelib::uint& blue, std::string& detail_value) const;
		void OnGreenParse(gamelib::uint& green, std::string& detail_value) const;
		void OnRedParse(gamelib::uint& red, std::string& detail_value) const;
		void OnColourKeyParse(bool& color_key_enabled, std::string& detail_value) const;
		void OnPosYParse(gamelib::uint& y, std::string& detail_value) const;
		void OnVisibleParse(bool& visible, std::string& detail_value) const;
		void OnPosXParse(gamelib::uint& x, std::string& detail_value) const;
		void OnNameParse(std::string& x, std::string& detail_value) const;
		void OnTypeParse(std::string& x, std::string& detail_value) const;
		void GetAssetForResourceIdParse(std::string& detail_value, std::shared_ptr<gamelib::Asset>& resource) const;

		std::shared_ptr<gamelib::GameObject> InitializeGameObject(std::string& name, std::string& type, gamelib::coordinate<int> position, bool IsVisible, std::shared_ptr<gamelib::Asset> asset) const;

		// Private constructor 
		GameObjectFactory() = default;

		// Error handling
		void ThrowCouldNotFindAssetException(std::shared_ptr<gamelib::Asset>& asset, std::string& detail_value) const;
	};
}