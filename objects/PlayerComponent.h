#pragma once
#include "Component.h"
#include "Player.h"
#include "scene/Room.h"

namespace gamelib
{
	class PlayerComponent : public component
	{
	public:
		PlayerComponent(std::string component_name, Player* player);

		Player* the_player;
		
		int room;
	};
}
