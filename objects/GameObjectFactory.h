#pragma once
#include <memory>
#include "tinyxml2.h"
#include "GameObject.h"
#include "common/aliases.h"
#include "asset/Asset.h"
namespace gamelib
{
	class ResourceManager;

	/// <summary>
	/// Constructs a Game Object from the details about a game object (usually from file)
	/// </summary>
	class GameObjectFactory
	{
	public:	
		GameObjectFactory(GameObjectFactory const&) = delete;	
		GameObjectFactory& operator=(GameObjectFactory const&)  = delete;
		~GameObjectFactory() = default;		

		static GameObjectFactory& GetInstance();

		/// <summary>
		/// Build a game object
		/// </summary>
		/// <param name="scene_object_xml">Raw Object XML element</param>
		/// <returns>GameObject</returns>
		std::shared_ptr<GameObject> BuildGameObject(tinyxml2::XMLElement* scene_object_xml) const;
		
	private:
		static GameObjectFactory& instance;

		// Parsing object attribute handlers
		void OnBlueParse(gamelib::uint& blue, std::string& detail_value) const;
		void OnGreenParse(gamelib::uint& green, std::string& detail_value) const;
		void OnRedParse(gamelib::uint& red, std::string& detail_value) const;
		void OnColourKeyParse(bool& color_key_enabled, std::string& detail_value) const;
		void OnPosYParse(gamelib::uint& y, std::string& detail_value) const;
		void OnVisibleParse(bool& visible, std::string& detail_value) const;
		void OnPosXParse(gamelib::uint& x, std::string& detail_value) const;
		void GetAssetForResourceIdParse(std::string& detail_value, std::shared_ptr<gamelib::Asset>& resource) const;

		/// <summary>
		/// Initializes the Game object
		/// </summary>
		std::shared_ptr<GameObject>& InitializeGameObject(std::shared_ptr<GameObject>& game_object, uint x, uint y, bool is_visible, std::shared_ptr<Asset>& resource, bool color_key_enabled, const uint& red, const uint& green, const uint& blue) const;
		
		// Private constructor 
		GameObjectFactory() = default;

		// Error handling
		void ThrowCouldNotFindAssetException(std::shared_ptr<gamelib::Asset>& asset, std::string& detail_value) const;
	};
}