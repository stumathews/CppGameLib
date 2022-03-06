#pragma once

#include "objects/Component.h"
#include <objects/game_world_data.h>
#include <memory>
#include "common/constants.h"

namespace gamelib
{
class game_world_component : public component
	{
		game_world_data&  data;
	public:
		game_world_component(game_world_data& world) : component(constants::game_world), data(world)
		{
		}

		game_world_data& get_data() const {return data;}
	};
}
