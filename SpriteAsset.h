#pragma once
#include "KeyFrame.h"
#include "graphic/GraphicAsset.h"
#include "scene/ABCDRectangle.h"
namespace gamelib
{
	class SpriteAsset : public GraphicAsset
	{
	public:
		SpriteAsset(
			const int uid, 
			const std::string& name, 
			const std::string& path, 
			const std::string& type,
			const int scene, 
			const ABCDRectangle& dimensions);
		float FrameDurationMs;
		std::vector<KeyFrame> KeyFrames;
	};
}

