#include "GameObjectFactory.h"
#include "GameObject.h"
#include "Sprite.h"
#include "resource/ResourceManager.h"
#include "scene/Room.h"
#include "common/StaticConfig.h"

using namespace tinyxml2;
using namespace std;

namespace gamelib
{

	/// <summary>
	/// Build Game object from XML
	/// </summary>
	/// <param name="scene_object_xml">XML element describing game object</param>
	/// <returns></returns>
	shared_ptr<GameObject> GameObjectFactory::BuildGameObject( 	XMLElement * scene_object_xml, 
																ResourceManager& resourceManager, 
																SettingsManager& settingsManager, 
																EventManager& eventManager) const
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

		for(const auto* sceneObjectAttribute = scene_object_xml->FirstAttribute(); sceneObjectAttribute; sceneObjectAttribute = sceneObjectAttribute->Next()) 
		{
			// Extract/Save details about the object

			string attributeName(sceneObjectAttribute->Name());
			string attributeValue(sceneObjectAttribute->Value());		
						
			if(attributeName == "resourceId") 
			{
				// This object is associated with a resource, so use the resource manager

				OnResourceIdParse(attributeValue, resourceManager, asset);
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
		return InitializeGameObject(emptyGameObject, x, y, isVisible, asset, isColourKeyEnabled, redValue, greenValue, blueValue, resourceManager, settingsManager, eventManager);		
	}

	/// <summary>
	/// Parse the Resource Id attribute for scene object
	/// </summary>
	void GameObjectFactory::OnResourceIdParse(std::string& detail_value, gamelib::ResourceManager& resourceManager, std::shared_ptr<gamelib::Asset>& resource) const
	{
		// Extract/Save resource Id from XML
		const auto* const resourceIdString = detail_value.c_str();

		// Turn it into an integer
		int resourceId = static_cast<int>(atoi(resourceIdString));

		// Get generic asset info about this resource
		auto asset = resourceManager.GetAssetInfo(resourceId);
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
	/// Create a game object from the resource details
	/// </summary>
	shared_ptr<GameObject>& GameObjectFactory::InitializeGameObject(shared_ptr<GameObject>& gameObject,
																	uint x,
																	uint y,
																	bool isVisible,
																	shared_ptr<Asset>& asset,
																	const bool colourKeyEnabled,
																	const uint& red,
																	const uint& green,
																	const uint& blue,
																	ResourceManager& resourceManager,
																	SettingsManager& settingsManager,
																	EventManager& eventManager) const
	{
		if (asset == nullptr)
		{
			throw exception("cannot initialize game object with out an associated asset");
		}
		
		// Initialize asset type = Graphic
		if (asset->type._Equal("graphic"))
		{		
			shared_ptr<GraphicAsset> graphic = dynamic_pointer_cast<GraphicAsset>(asset);
			// If no graphic resource is associated with this game object, we'll make it a simple geometric Room object
			bool isGraphicValid = graphic != nullptr;

			if (!isGraphicValid)
			{
				throw exception("Not supported resource type");
			}

			if ((red < 0 || red > 255) || (blue < 0 || blue > 255) || (green < 0 || green > 255))
			{
				throw exception("Invalid color values when constructing game object");
			}

			auto framesPerRow = StaticConfig::frames_per_row; // number of frames in a row in the sprite sheet
			auto framesPerColumn = StaticConfig::frames_per_column; // number of frames in a column in the sprite sheet
			auto numKeyFrames = 0;
			auto speed = 0;
			auto keyFrameWidth = 0;
			auto keyFrameHeight = 0;

			// Is it an animated Sprite
			if (graphic->IsAnimated())
			{
				// Sprite
				numKeyFrames = graphic->GetNumKeyFrames();
				auto spriteWidth = settingsManager.get_int("global", "sprite_width"); // Should probably be reading this from the object details in the resource file/object
				speed = spriteWidth;
				keyFrameWidth = graphic->GetKeyFrameWidth();
				keyFrameHeight = graphic->GetKeyFrameHeight();
			}
			else
			{
				// 2D Sprite (with no animation)
				numKeyFrames = 1;
				speed = 1;
				keyFrameWidth = graphic->GetKeyFrameWidth();
				keyFrameHeight = 1;
			}

			// An Animated Sprite can represent a moving (animated) sprite or a static (non-moving) sprite
			gameObject = shared_ptr<AnimatedSprite>(new AnimatedSprite(x, y, speed, numKeyFrames, framesPerRow, framesPerColumn, keyFrameWidth, keyFrameHeight, isVisible, settingsManager, eventManager));

			if (graphic->IsAnimated())
			{
				// Start the animation for the animated sprite
				dynamic_pointer_cast<AnimatedSprite>(gameObject)->PlayAnimation();
			}

			// Set the graphic on the game object
			gameObject->SetGraphic(graphic);
		}
		else
		{
			auto message = string("Not supported resource type:") + asset->type;
			throw exception(message.c_str());
		}

		// Load the game objects individual settings
		gameObject->LoadSettings(settingsManager);

		// Set the colour key on the game object
		gameObject->isColorKeyEnabled = colourKeyEnabled;

		// Set the game object's visibility
		gameObject->isVisible = isVisible;

		if (gameObject->isColorKeyEnabled)
		{
			gameObject->SetColourKey(red, green, blue);
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


