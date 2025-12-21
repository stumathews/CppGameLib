#include "asset/asset.h"
#include "GameObjectFactory.h"
#include "GameObject.h"
#include "character/AnimatedSprite.h"
#include "resource/ResourceManager.h"
#include "common/aliases.h"
#include "asset/SpriteAsset.h"
#include "character/StaticSprite.h"
#include <exceptions/EngineException.h>

using namespace tinyxml2;
using namespace std;

namespace gamelib
{
	GameObjectFactory& GameObjectFactory::Get() { static GameObjectFactory instance; return instance; }

	shared_ptr<GameObject> GameObjectFactory::BuildGameObject(const XMLElement * sceneObjectXml) const
	{
		uint redValue = 0, greenValue = 0, blueValue = 0;
		uint x = 0, y = 0;
		string name;
		const string type;
		auto isVisible = false;
		shared_ptr<Asset> asset;		

		for(const auto* attribute = sceneObjectXml->FirstAttribute(); attribute; attribute = attribute->Next()) 
		{
			string attributeName = attribute->Name();
			string attributeValue = attribute->Value();		
						
			if (attributeName == "resourceId") { GetAssetForResourceIdParse(attributeValue, /*out*/ asset); continue; }
			if (attributeName == "posx") { OnPosXParse(x, attributeValue); continue; }
			if (attributeName == "visible") { OnVisibleParse(isVisible, attributeValue); continue; }
			if (attributeName == "posy") { OnPosYParse(y, attributeValue); continue; }
			if (attributeName == "r") { OnRedParse(redValue, attributeValue); continue; }
			if (attributeName == "g") { OnGreenParse(greenValue, attributeValue); continue; }
			if (attributeName == "b") { OnBlueParse(blueValue, attributeValue); continue; }
			if (attributeName == "name") { OnNameParse(name, attributeValue); continue; }
			if (attributeName == "type") { OnTypeParse(name, attributeValue); continue; }
		}
				
		return InitializeGameObject(name, type, Coordinate<int>(static_cast<int>(x), static_cast<int>(y)), isVisible, asset);		
	}


	void GameObjectFactory::GetAssetForResourceIdParse(const std::string& detailValue, std::shared_ptr<Asset>& resource)
	{

		const auto* const resourceIdString = detailValue.c_str();
		const int resourceId = static_cast<int>(atoi(resourceIdString));
		const auto asset = ResourceManager::Get()->GetAssetInfo(resourceId);

		if (asset == nullptr) 
		{
			THROW(99, "Resource manager could not determine the asset", "GameObjectFactory");
		}

		ThrowCouldNotFindAssetException(asset, detailValue);

		if (asset->Type != "graphic") { THROW(99,"Cannot load non graphic asset yet...", "GameObjectFactory"); }

		resource = asset;
	}

	shared_ptr<GameObject> GameObjectFactory::InitializeGameObject(const string& name, const string& type, const Coordinate<int> position, const bool IsVisible, const shared_ptr<Asset>
	                                                               & asset) const
	{
		if (asset == nullptr) { THROW(99,"cannot initialize game object with out an associated asset","GameObjectFactory"); }
		
		switch (asset->AssetType)  // NOLINT(clang-diagnostic-switch-enum)
		{
			case Asset::AssetType::Sprite:  return BuildSprite(name, type, asset, position, IsVisible);
			case Asset::AssetType::Graphic: return BuildGraphic(asset, position);
			default: THROW(99,(std::string("Graphic asset not supported:") + asset->Type).c_str(), "GameObjectFactory");
		}
	}

	shared_ptr<StaticSprite> GameObjectFactory::BuildGraphic(const std::shared_ptr<Asset>
	                                                         & asset, const Coordinate<int>& position) const
	{
		
		const auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);

		// graphic assets have dimensions, so we will need it as a graphic asset
		const auto spriteAsset = make_shared<SpriteAsset>(graphicAsset->Uid, graphicAsset->Name,
		                                                  graphicAsset->FilePath, graphicAsset->Type,
		                                                  graphicAsset->SceneId, graphicAsset->Dimensions);

		return Get().BuildStaticSprite(spriteAsset, position);

	}

	std::shared_ptr<StaticSprite> GameObjectFactory::BuildStaticSprite(const std::shared_ptr<Asset>& asset, const Coordinate<int>& position) const
	{
		return StaticSprite::Create(position, dynamic_pointer_cast<SpriteAsset>(asset));
	}

	shared_ptr<AnimatedSprite> GameObjectFactory::BuildSprite(const string& name, const string& type, const std::shared_ptr<Asset>
	                                                          & asset, const Coordinate<int>& position, const bool isVisible) 
	{
		const auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(asset);
		const auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);
		auto sprite = AnimatedSprite::Create(position, spriteAsset);

		SetupCommonSprite(sprite, asset, graphicAsset, isVisible);

		sprite->KeyFrames = spriteAsset->KeyFrames;
		sprite->PlayAnimation();	

		return sprite;
	}

	void GameObjectFactory::SetupCommonSprite(const std::shared_ptr<AnimatedSprite>& sprite, const std::shared_ptr<Asset>& asset, const std::shared_ptr<
		                                          GraphicAsset>& graphicAsset, const bool isVisible)
	{
		sprite->SetTag(asset->Name);
		sprite->SetGraphic(graphicAsset);
		sprite->LoadSettings();
		sprite->SupportsColourKey(graphicAsset->GetColourKey().IsSet());
	}

	void GameObjectFactory::OnBlueParse(uint& blue, const std::string& detailValue) const { blue = stoi(detailValue); }
	void GameObjectFactory::OnGreenParse(uint& green, const std::string& detailValue) const { green = stoi(detailValue); }
	void GameObjectFactory::OnRedParse(uint& red, const std::string& detailValue) const { red = stoi(detailValue); }
	void GameObjectFactory::OnColourKeyParse(bool& color_key_enabled, const std::string& detailValue) { color_key_enabled = (detailValue == "true"); }
	void GameObjectFactory::OnPosYParse(uint& y, const std::string& detailValue) const { y = stoi(detailValue); }
	void GameObjectFactory::OnVisibleParse(bool& visible, const std::string& detailValue) { visible = (detailValue == "true"); }
	void GameObjectFactory::OnPosXParse(uint& x, const std::string& detailValue) const { x = stoi(detailValue); }
	void GameObjectFactory::OnNameParse(string& x, const std::string& detailValue) { x = detailValue; }
	void GameObjectFactory::OnTypeParse(string& x, const std::string& detailValue) { x = detailValue; }

	void GameObjectFactory::ThrowCouldNotFindAssetException(const std::shared_ptr<Asset>& asset, const std::string& detailValue)
	{
		if (asset->Type != "graphic") { 
			THROW(99, "Cannot load non graphic resource: ", "GameObjectFactory"); }
		if (asset == nullptr) { 
			THROW(99,"Could not load resource meta data for resource id:", "GameObjectFactory"); }
	}	
}


