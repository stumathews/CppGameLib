#pragma once

#include "character/Component.h"
#include <objects/GameWorldData.h>
#include "common/constants.h"

namespace gamelib
{

class GameWorldComponent : public Component
	{
		GameWorldData&  data;
	public:
		explicit GameWorldComponent(GameWorldData& gameWorld)
	: Component(constants::GameWorld), data(gameWorld)
		{
		}

		[[nodiscard]] GameWorldData& get_data() const {return data;}
	};
}
