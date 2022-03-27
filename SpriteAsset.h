#pragma once
#include "KeyFrame.h"
#include "graphic/GraphicAsset.h"
#include "scene/ABCDRectangle.h"
namespace gamelib
{
	class SpriteAsset : public GraphicAsset
	{
	public:
		SpriteAsset(int uid, std::string& name, const std::string& path, const std::string& type, int scene, ABCDRectangle& dimensions);
		std::vector<KeyFrame> KeyFrames;
	};
}

