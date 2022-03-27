#pragma once

#include "objects/Component.h"
#include <objects/GameWorldData.h>
#include <memory>
#include "common/constants.h"

namespace gamelib
{
class game_world_component : public Component
	{
		GameWorldData&  data;
	public:
		game_world_component(GameWorldData& gameWorld) : Component(constants::game_world), data(gameWorld)
		{
		}

		GameWorldData& get_data() const {return data;}
	};
}
