#pragma once
#include "Component.h"
#include "scene/Square.h"

namespace gamelib
{
	class player_component : public component
	{
	public:
		player_component(std::string name, int x, int y, int w, int h) : component(name), x(x), y(y), w(w), h(h)
		{
		}
		int x, y, w, h;
		int room;
	};
}
