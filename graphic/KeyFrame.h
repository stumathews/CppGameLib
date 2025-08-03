#pragma once
#include <string>

namespace gamelib
{
	class KeyFrame
	{
	public:
		KeyFrame(int x, int y, int w, int h, std::string group);
		int X, Y, W, H;
		std::string Group;
		[[nodiscard]] bool HasGroup() const;
	};
}

