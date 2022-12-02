#include "GameObjectFactory.h"
#include "GameObject.h"
#include "AnimatedSprite.h"
#include "resource/ResourceManager.h"
#include "common/StaticConfig.h"
#include "common/aliases.h"
#include "SpriteAsset.h"

using namespace tinyxml2;
using namespace std;

namespace gamelib
{
	GameObjectFactory& GameObjectFactory::Get() { static GameObjectFactory instance; return instance; }

	shared_ptr<GameObject> GameObjectFactory::BuildGameObject(XMLElement * objectElement) const
	{
		uint redValue, greenValue, blueValue;
		redValue = greenValue = blueValue = 0;
		uint x = 0;
		uint y = 0;
		auto IsVisible = false;
		auto isColourKeyEnabled = false;
		shared_ptr<Asset> asset;		
		auto templateObject = shared_ptr<GameObject>(nullptr);

		for(const auto* attribute = objectElement->FirstAttribute(); attribute; attribute = attribute->Next()) 
		{
			string attributeName(attribute->Name());
			string attributeValue(attribute->Value());		
						
			if (attributeName == "resourceId") { GetAssetForResourceIdParse(attributeValue, /*out*/ asset); continue; }
			if (attributeName == "posx") { OnPosXParse(x, attributeValue); continue; }
			if (attributeName == "visible") { OnVisibleParse(IsVisible, attributeValue); continue; }
			if (attributeName == "posy") { OnPosYParse(y, attributeValue); continue; }
			if (attributeName == "colourKey") { OnColourKeyParse(isColourKeyEnabled, attributeValue); continue; }
			if (attributeName == "r") { OnRedParse(redValue, attributeValue); continue; }
			if (attributeName == "g") { OnGreenParse(greenValue, attributeValue); continue; }
			if (attributeName == "b") { OnBlueParse(blueValue, attributeValue); continue; }
		}

		auto colourKey = gamelib::ColourKey(redValue, greenValue, blueValue);

		return InitializeGameObject(templateObject, coordinate<int>(x, y), IsVisible, asset, colourKey);
	}


	void GameObjectFactory::GetAssetForResourceIdParse(std::string& detail_value, std::shared_ptr<gamelib::Asset>& resource) const
	{

		const auto* const resourceIdString = detail_value.c_str();
		int resourceId = static_cast<int>(atoi(resourceIdString));
		auto asset = ResourceManager::Get()->GetAssetInfo(resourceId);
		if (asset == nullptr) 
		{
			throw exception("Resouce manager could not determine the asset");
		}
		else
		{
			ThrowCouldNotFindAssetException(asset, detail_value);

			if (!asset->type._Equal("graphic")) { throw exception("Cannot load non graphic asset yet..."); }

			resource = asset;
		}
	}

	shared_ptr<GameObject>& GameObjectFactory::InitializeGameObject(shared_ptr<GameObject>& templateObject, coordinate<int> position, bool IsVisible, shared_ptr<Asset>& asset, ColourKey colourKey) const
	{
		if (asset == nullptr) { throw exception("cannot initialize game object with out an associated asset"); }
		if ((colourKey.Red < 0 || colourKey.Red > 255) || (colourKey.Blue < 0 || colourKey.Blue > 255) || (colourKey.Green < 0 || colourKey.Green > 255)) { throw exception("Invalid color values when constructing game object"); }
		
		switch (asset->assetType)
		{
			case Asset::AssetType::Sprite:  BuildSprite(asset, position, IsVisible, colourKey, /* (out/in) */ templateObject); break;
			case Asset::AssetType::Graphic: BuildGraphic(asset, position, IsVisible, colourKey, /* (out/in) */ templateObject); break;
			default: 
				throw exception((string("Graphic asset not supported:") + asset->type).c_str()); break; 
		}

		return templateObject;
	}

	void GameObjectFactory::BuildGraphic(std::shared_ptr<gamelib::Asset>& asset, const gamelib::coordinate<int>& position, bool IsVisible, gamelib::ColourKey& colourKey, std::shared_ptr<gamelib::GameObject>& templateObject) const
	{
		auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);
		auto _sprite = shared_ptr<AnimatedSprite>(new AnimatedSprite(position, 100, IsVisible, graphicAsset->Dimensions));

		SetupCommonSprite(_sprite, asset, graphicAsset, colourKey, IsVisible);

		_sprite->AdjustViewportToCurrentDimensions();

		templateObject = _sprite;
	}

	void GameObjectFactory::BuildSprite(std::shared_ptr<gamelib::Asset>& asset, const gamelib::coordinate<int>& position, bool IsVisible, gamelib::ColourKey& colourKey, std::shared_ptr<gamelib::GameObject>& templateObject) const
	{
		auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(asset);
		auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);
		auto _sprite = shared_ptr<AnimatedSprite>(new AnimatedSprite(position, 100, IsVisible, spriteAsset->Dimensions));

		SetupCommonSprite(_sprite, asset, graphicAsset, colourKey, IsVisible);

		_sprite->KeyFrames = spriteAsset->KeyFrames;
		_sprite->PlayAnimation();	

		templateObject = _sprite;
	}

	void GameObjectFactory::SetupCommonSprite(std::shared_ptr<gamelib::AnimatedSprite>& _sprite, std::shared_ptr<gamelib::Asset>& asset, std::shared_ptr<gamelib::GraphicAsset>& graphicAsset, gamelib::ColourKey& colourKey, bool IsVisible) const
	{
		_sprite->SetTag(asset->name);
		_sprite->SetGraphic(graphicAsset);
		_sprite->LoadSettings();
		_sprite->SupportsColourKey(colourKey.IsSet());
		_sprite->IsVisible = IsVisible;

		// Tell the render what colour it should consider as transparent i.e ignore drawing
		if (_sprite->HasColourKey()) { _sprite->SetColourKey(colourKey.Red, colourKey.Green, colourKey.Blue); }
	}

	void GameObjectFactory::OnBlueParse(gamelib::uint& blue, std::string& detail_value) const { blue = stoi(detail_value); }
	void GameObjectFactory::OnGreenParse(gamelib::uint& green, std::string& detail_value) const { green = stoi(detail_value); }
	void GameObjectFactory::OnRedParse(gamelib::uint& red, std::string& detail_value) const { red = stoi(detail_value); }
	void GameObjectFactory::OnColourKeyParse(bool& color_key_enabled, std::string& detail_value) const { color_key_enabled = detail_value._Equal("true") ? true : false; }
	void GameObjectFactory::OnPosYParse(gamelib::uint& y, std::string& detail_value) const { y = stoi(detail_value); }
	void GameObjectFactory::OnVisibleParse(bool& visible, std::string& detail_value) const { visible = detail_value._Equal("true") ? true : false; }
	void GameObjectFactory::OnPosXParse(gamelib::uint& x, std::string& detail_value) const { x = stoi(detail_value); }

	void GameObjectFactory::ThrowCouldNotFindAssetException(std::shared_ptr<gamelib::Asset>& asset, std::string& detail_value) const
	{
		if (!asset->type._Equal("graphic")) { throw exception(("Cannot load non graphic resource: " + asset->name + " type=" + asset->type).c_str()); }
		if (asset == nullptr) { throw exception(("Could not load resource meta data for resource id:" + detail_value).c_str()); }
	}	
}


