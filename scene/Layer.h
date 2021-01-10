#pragma once
#include <string>
#include <list>
#include <memory>

namespace gamelib
{
	class GameObject;

	class layer
	{
	public:
		bool visible = false;
		unsigned int zorder = 0;
		float x = 0;
		float y = 0;
		std::string name;
		std::list<std::shared_ptr<GameObject>> game_objects;
	};
}


