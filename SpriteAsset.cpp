#include "SpriteAsset.h"
namespace gamelib
{
	SpriteAsset::SpriteAsset(int uid, std::string& name, const std::string& path, const std::string& type, int scene, ABCDRectangle& dimensions)
		: GraphicAsset(uid, name, path, type, scene, dimensions)
	{
		assetType = Asset::AssetType::Sprite;
	}
}
