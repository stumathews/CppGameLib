#pragma once

#include "objects/Component.h"
#include <objects/game_world_data.h>
#include <memory>
#include "common/constants.h"

namespace gamelib
{
class game_world_component : public component
	{
		std::shared_ptr<game_world_data>  data;
	public:
		game_world_component(std::shared_ptr<game_world_data> world) : component(constants::game_world)
		{
			data = world;
		}

		std::shared_ptr<game_world_data> get_data() const {return data;}
	};
}
