#pragma once
#include <string>
namespace gamelib
{
	class KeyFrame
	{
	public:
		KeyFrame(int x, int y, int w, int h, std::string group);
		int x, y, w, h;
		std::string group;
		bool HasGroup();
	};
}

