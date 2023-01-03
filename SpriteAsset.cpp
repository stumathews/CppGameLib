#include "SpriteAsset.h"
namespace gamelib
{
	SpriteAsset::SpriteAsset(const int inUid, const std::string& inName, const std::string& inPath,
	                         const std::string& inType, const int inScene, const ABCDRectangle& dimensions)
		: GraphicAsset(inUid, inName, inPath, inType, inScene, dimensions)
	{
		assetType = AssetType::Sprite;
		FrameDurationMs = 0;
		
	}
}
