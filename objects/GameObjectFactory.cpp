#include "GameObjectFactory.h"
#include "GameObject.h"
#include "AnimatedSprite.h"
#include "resource/ResourceManager.h"
#include "common/aliases.h"
#include "SpriteAsset.h"
#include "StaticSprite.h"

using namespace tinyxml2;
using namespace std;

namespace gamelib
{
	GameObjectFactory& GameObjectFactory::Get() { static GameObjectFactory instance; return instance; }

	shared_ptr<GameObject> GameObjectFactory::BuildGameObject(const XMLElement * objectElement) const
	{
		uint redValue = 0, greenValue = 0, blueValue = 0;
		uint x = 0, y = 0;
		string name;
		const string type;
		auto IsVisible = false;
		shared_ptr<Asset> asset;		

		for(const auto* attribute = objectElement->FirstAttribute(); attribute; attribute = attribute->Next()) 
		{
			string attributeName = attribute->Name();
			string attributeValue = attribute->Value();		
						
			if (attributeName == "resourceId") { GetAssetForResourceIdParse(attributeValue, /*out*/ asset); continue; }
			if (attributeName == "posx") { OnPosXParse(x, attributeValue); continue; }
			if (attributeName == "visible") { OnVisibleParse(IsVisible, attributeValue); continue; }
			if (attributeName == "posy") { OnPosYParse(y, attributeValue); continue; }
			if (attributeName == "r") { OnRedParse(redValue, attributeValue); continue; }
			if (attributeName == "g") { OnGreenParse(greenValue, attributeValue); continue; }
			if (attributeName == "b") { OnBlueParse(blueValue, attributeValue); continue; }
			if (attributeName == "name") { OnNameParse(name, attributeValue); continue; }
			if (attributeName == "type") { OnTypeParse(name, attributeValue); continue; }
		}
				
		return InitializeGameObject(name, type, Coordinate<int>(static_cast<int>(x), static_cast<int>(y)), IsVisible, asset);		
	}


	void GameObjectFactory::GetAssetForResourceIdParse(std::string& detail_value, std::shared_ptr<Asset>& resource)
	{

		const auto* const resourceIdString = detail_value.c_str();
		const int resourceId = static_cast<int>(atoi(resourceIdString));
		auto asset = ResourceManager::Get()->GetAssetInfo(resourceId);
		if (asset == nullptr) 
		{
			throw exception("Resouce manager could not determine the asset");
		}
		ThrowCouldNotFindAssetException(asset, detail_value);

		if (!asset->type._Equal("graphic")) { throw exception("Cannot load non graphic asset yet..."); }

		resource = asset;
	}

	shared_ptr<GameObject> GameObjectFactory::InitializeGameObject(const string& name, const string& type, const Coordinate<int> position, const bool IsVisible, const shared_ptr<Asset>
	                                                               & asset) const
	{
		if (asset == nullptr) { throw exception("cannot initialize game object with out an associated asset"); }
		
		switch (asset->assetType)  // NOLINT(clang-diagnostic-switch-enum)
		{
			case Asset::AssetType::Sprite:  return BuildSprite(name, type, asset, position, IsVisible);
			case Asset::AssetType::Graphic: return BuildGraphic(name, type, asset, position, IsVisible);
			default: throw exception((string("Graphic asset not supported:") + asset->type).c_str());
		}
	}

	shared_ptr<StaticSprite> GameObjectFactory::BuildGraphic(const string& name, const string& type, const std::shared_ptr<Asset>
	                                                         & asset, const Coordinate<int>& position, const bool IsVisible) const
	{
		
		const auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);

		// graphic assets have dimensions, so we will need it as a graphic asset
		const auto spriteAsset = make_shared<SpriteAsset>(graphicAsset->uid, graphicAsset->name,
		                                                  graphicAsset->path, graphicAsset->type,
		                                                  graphicAsset->scene, graphicAsset->Dimensions);

		return Get().BuildStaticSprite(name, type, spriteAsset, position);

	}

	std::shared_ptr<StaticSprite> GameObjectFactory::BuildStaticSprite(const std::string& name, const std::string& type, const std::shared_ptr<
		                                                                   Asset>
	                                                                   & asset, const Coordinate<int>& position) const
	{
		return StaticSprite::Create(position, dynamic_pointer_cast<SpriteAsset>(asset));
	}

	shared_ptr<AnimatedSprite> GameObjectFactory::BuildSprite(const string& name, const string& type, const std::shared_ptr<Asset> asset, const Coordinate<int>& position, const bool IsVisible) const
	{
		const auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(asset);
		const auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);
		auto _sprite = std::make_shared<AnimatedSprite>(name, type, position, 100, IsVisible, spriteAsset);
		SetupCommonSprite(_sprite, asset, graphicAsset, IsVisible);

		_sprite->KeyFrames = spriteAsset->KeyFrames;
		_sprite->PlayAnimation();	

		return _sprite;
	}

	void GameObjectFactory::SetupCommonSprite(const std::shared_ptr<AnimatedSprite>& _sprite, const std::shared_ptr<Asset>& asset, const std::shared_ptr<
		                                          GraphicAsset>& graphicAsset, const bool IsVisible)
	{
		_sprite->SetTag(asset->name);
		_sprite->SetGraphic(graphicAsset);
		_sprite->LoadSettings();
		_sprite->SupportsColourKey(graphicAsset->GetColourKey().IsSet());
		_sprite->IsVisible = IsVisible;

		// Tell the render what colour it should consider as transparent i.e ignore drawing
		if (_sprite->HasColourKey())
		{
			_sprite->SetColourKey(graphicAsset->GetColourKey().Red, graphicAsset->GetColourKey().Green,
			                      graphicAsset->GetColourKey().Blue);
		}
	}

	void GameObjectFactory::OnBlueParse(uint& blue, const std::string& detailValue) const { blue = stoi(detailValue); }
	void GameObjectFactory::OnGreenParse(uint& green, std::string& detail_value) const { green = stoi(detail_value); }
	void GameObjectFactory::OnRedParse(uint& red, std::string& detail_value) const { red = stoi(detail_value); }
	void GameObjectFactory::OnColourKeyParse(bool& color_key_enabled, std::string& detail_value) { color_key_enabled = detail_value._Equal("true") ? true : false; }
	void GameObjectFactory::OnPosYParse(uint& y, std::string& detail_value) const { y = stoi(detail_value); }
	void GameObjectFactory::OnVisibleParse(bool& visible, std::string& detail_value) { visible = detail_value._Equal("true") ? true : false; }
	void GameObjectFactory::OnPosXParse(uint& x, std::string& detail_value) const { x = stoi(detail_value); }
	void GameObjectFactory::OnNameParse(string& x, std::string& detail_value) { x = detail_value; }
	void GameObjectFactory::OnTypeParse(string& x, std::string& detail_value) { x = detail_value; }

	void GameObjectFactory::ThrowCouldNotFindAssetException(std::shared_ptr<Asset>& asset, std::string& detail_value)
	{
		if (!asset->type._Equal("graphic")) { throw exception(("Cannot load non graphic resource: " + asset->name + " type=" + asset->type).c_str()); }
		if (asset == nullptr) { throw exception(("Could not load resource meta data for resource id:" + detail_value).c_str()); }
	}	
}


