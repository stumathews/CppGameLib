#include "SpriteAsset.h"
namespace gamelib
{
	SpriteAsset::SpriteAsset(const int uid, const std::string& name, const std::string& path, const std::string& type, const int scene, const ABCDRectangle& dimensions)
		: GraphicAsset(uid, name, path, type, scene, dimensions)
	{
		assetType = Asset::AssetType::Sprite;
		FrameDurationMs = 0;
		
	}
}
