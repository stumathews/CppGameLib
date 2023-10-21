#pragma once
#include "graphic/KeyFrame.h"
#include "graphic/GraphicAsset.h"
#include "geometry/ABCDRectangle.h"
namespace gamelib
{
	class SpriteAsset final : public GraphicAsset
	{
	public:
		SpriteAsset(
			int inUid, 
			const std::string& inName, 
			const std::string& inPath, 
			const std::string& inType,
			int inScene, 
			const AbcdRectangle& dimensions);
		float FrameDurationMs;
		std::vector<KeyFrame> KeyFrames;
	};
}

