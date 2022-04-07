#include "GameObjectFactory.h"
#include "GameObject.h"
#include "Sprite.h"
#include "resource/ResourceManager.h"
#include "common/StaticConfig.h"
#include "common/aliases.h"
#include "SpriteAsset.h"

using namespace tinyxml2;
using namespace std;

namespace gamelib
{

	/// <summary>
	/// Build Game object from XML
	/// </summary>
	/// <param name="scene_object_xml">XML element describing game object</param>
	/// <returns></returns>
	shared_ptr<GameObject> GameObjectFactory::BuildGameObject(XMLElement * sceneObject) const
	{
		// Game Object details:
		uint redValue = 0;
		uint greenValue = 0;
		uint blueValue = 0;
		uint x = 0;
		uint y = 0;
		auto isVisible = false;
		auto isColourKeyEnabled = false;

		// Generic asset we will construct
		shared_ptr<Asset> asset;
		
		auto emptyGameObject = shared_ptr<GameObject>(nullptr);

		for(const auto* sceneObjectAttribute = sceneObject->FirstAttribute(); sceneObjectAttribute; sceneObjectAttribute = sceneObjectAttribute->Next()) 
		{
			// Extract/Save details about the object

			string attributeName(sceneObjectAttribute->Name());
			string attributeValue(sceneObjectAttribute->Value());		
						
			if(attributeName == "resourceId") 
			{
				// This object is associated with a resource, so use the resource manager

				GetAssetForResourceIdParse(attributeValue, /*out*/ asset);
				continue; // Move to the next attribute on element
			}
			
			// Object's initial x position
			if(attributeName == "posx")
			{
				OnPosXParse(x, attributeValue);
				continue; // Move to the next attribute on element
			}

			// Object initial visibility setting
			if(attributeName == "visible")
			{
				OnVisibleParse(isVisible, attributeValue);
				continue; // Move to the next attribute on element
			}

			// object initial y position
			if(attributeName == "posy")
			{
				OnPosYParse(y, attributeValue);
				continue; // Move to the next attribute on element
			}

			// object's associated colour key
			if(attributeName == "colourKey") 
			{
				OnColourKeyParse(isColourKeyEnabled, attributeValue);
				continue; // Move to the next attribute on element
			}

			// Objects red color
			if(attributeName._Equal("r")) 
			{
				OnRedParse(redValue, attributeValue);
				continue; // Move to the next attribute on element
			}

			// Objects green color
			if(attributeName == "g") 
			{
				OnGreenParse(greenValue, attributeValue);
				continue; // Move to the next attribute on element
			}

			// Objects blue color
			if (attributeName == "b")
			{
				OnBlueParse(blueValue, attributeValue);
				continue; // Move to the next attribute on element
			}
		}
			
		// Take all the discovered details about the scene object and make a Game Object fom it
		return InitializeGameObject(emptyGameObject, x, y, isVisible, asset, isColourKeyEnabled, redValue, greenValue, blueValue);		
	}

	/// <summary>
	/// Parse the Resource Id attribute for scene object
	/// </summary>
	void GameObjectFactory::GetAssetForResourceIdParse(std::string& detail_value, std::shared_ptr<gamelib::Asset>& resource) const
	{
		// Extract/Save resource Id from XML
		const auto* const resourceIdString = detail_value.c_str();

		// Turn it into an integer
		int resourceId = static_cast<int>(atoi(resourceIdString));

		// Get generic asset info about this resource
		auto asset = ResourceManager::Get()->GetAssetInfo(resourceId);
		if (asset == nullptr)
		{
			throw exception("Resouce manager could not determine the asset");
		}
		else
		{
			GetInstance().ThrowCouldNotFindAssetException(asset, detail_value);

			if (!asset->type._Equal("graphic"))
			{
				throw exception("Cannot load non graphic asset yet...");
			}

			resource = asset;
		}
	}

	/// <summary>
	/// Create a game object from the Asset details
	/// </summary>
	shared_ptr<GameObject>& GameObjectFactory::InitializeGameObject(shared_ptr<GameObject>& gameObject,
																	uint x,
																	uint y,
																	bool isVisible,
																	shared_ptr<Asset>& asset,
																	const bool colourKeyEnabled,
																	const uint& red,
																	const uint& green,
																	const uint& blue) const
	{
		if (asset == nullptr)
		{
			throw exception("cannot initialize game object with out an associated asset");
		}

		if ((red < 0 || red > 255) || (blue < 0 || blue > 255) || (green < 0 || green > 255))
		{
			throw exception("Invalid color values when constructing game object");
		}
		
		if (asset->assetType == Asset::AssetType::Sprite)
		{
			auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(asset);
			auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);
			auto sprite = shared_ptr<AnimatedSprite>(new AnimatedSprite(x, y, 100, isVisible, spriteAsset->Dimensions));
			
			sprite->SetTag(spriteAsset->name);
			// Set the AnimatedSprite's key frames
			sprite->KeyFrames = spriteAsset->KeyFrames;	

			// Set the graphic asset of the Animated Sprite
			sprite->SetGraphic(graphicAsset);

			// Start playing the animation
			sprite->PlayAnimation();

			// Load the game objects individual settings
			sprite->LoadSettings();

			// Set the colour key on the game object
			sprite->SupportsColourKey(colourKeyEnabled);

			// Set the game object's visibility
			sprite->isVisible = isVisible;

			// Tell the render what colour it should consider as transparent i.e ignore drawing
			if (sprite->HasColourKey())
			{
				sprite->SetColourKey(red, green, blue);
			}

			gameObject = sprite;
		}
		else if(asset->assetType == Asset::AssetType::Graphic)
		{
			auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);
			auto sprite = shared_ptr<AnimatedSprite>(new AnimatedSprite(x, y, 100, isVisible, graphicAsset->Dimensions));
			
			// Set underlying graphic
			sprite->SetGraphic(graphicAsset);

			// Show the entire graphic
			sprite->AdjustViewportToCurrentDimensions();

			// Load the game objects individual settings
			sprite->LoadSettings();

			// Set the colour key on the game object
			sprite->SupportsColourKey(colourKeyEnabled);

			// Set the game object's visibility
			sprite->isVisible = isVisible;

			// Tell the render what colour it should consider as transparent i.e ignore drawing
			if (sprite->HasColourKey())
			{
				sprite->SetColourKey(red, green, blue);
			}

			gameObject = sprite;
		}
		else
		{
			auto message = string("Graphic asset not supported:") + asset->type;
			throw exception(message.c_str());
		}

		

		return gameObject;
	}


	/// <summary>
	/// Parse the blue attribute for scene object
	/// </summary>
	void GameObjectFactory::OnBlueParse(gamelib::uint& blue, std::string& detail_value) const
	{
		blue = stoi(detail_value);
	}

	/// <summary>
	/// Parse the green attribute for scene object
	/// </summary>
	void GameObjectFactory::OnGreenParse(gamelib::uint& green, std::string& detail_value) const
	{
		green = stoi(detail_value);
	}

	/// <summary>
	/// Parse the red attribute for scene object
	/// </summary>
	void GameObjectFactory::OnRedParse(gamelib::uint& red, std::string& detail_value) const
	{
		red = stoi(detail_value);
	}

	/// <summary>
	/// Parse the colour key attribute for scene object
	/// </summary>
	void GameObjectFactory::OnColourKeyParse(bool& color_key_enabled, std::string& detail_value) const
	{
		color_key_enabled = detail_value._Equal("true") ? true : false;
	}

	/// <summary>
	/// Parse the y attribute for scene object
	/// </summary>
	void GameObjectFactory::OnPosYParse(gamelib::uint& y, std::string& detail_value) const
	{
		y = stoi(detail_value);
	}

	/// <summary>
	/// Parse the visible attribute for scene object
	/// </summary>
	void GameObjectFactory::OnVisibleParse(bool& visible, std::string& detail_value) const
	{
		visible = detail_value._Equal("true") ? true : false;
	}

	/// <summary>
	/// Parse the x attribute for scene object
	/// </summary>
	void GameObjectFactory::OnPosXParse(gamelib::uint& x, std::string& detail_value) const
	{
		x = stoi(detail_value);
	}

	/// <summary>
	/// Could not find asset error
	/// </summary>
	void GameObjectFactory::ThrowCouldNotFindAssetException(std::shared_ptr<gamelib::Asset>& asset, std::string& detail_value) const
	{
		if (!asset->type._Equal("graphic"))
		{
			throw exception(("Cannot load non graphic resource: " + asset->name + " type=" + asset->type).c_str());
		}

		if (asset == nullptr)
		{
			throw exception(("Could not load resource meta data for resource id:" + detail_value).c_str());
		}
	}
	
	/// <summary>
	/// Singleton
	/// </summary>
	/// <returns>GameObjectFactory</returns>
	GameObjectFactory& GameObjectFactory::GetInstance()
	{
		static GameObjectFactory instance;
		return instance;
	}

	
}


