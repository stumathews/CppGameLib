#pragma once
#include <string>
#include <list>
#include <memory>

namespace gamelib
{
	class game_object;

	class layer
	{
	public:
		bool visible = false;
		unsigned int zorder = 0;
		float x = 0;
		float y = 0;
		std::string name;
		std::list<std::shared_ptr<game_object>> game_objects;
	};
}


