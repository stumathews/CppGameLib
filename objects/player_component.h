#pragma once
#include "Component.h"
#include "Player.h"
#include "scene/Room.h"

namespace gamelib
{
	class player_component : public component
	{
	public:
		player_component(std::string component_name, Player* player);

		Player* the_player;
		
		int room;
	};
}
