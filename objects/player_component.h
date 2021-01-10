#pragma once
#include "Component.h"
#include "Player.h"
#include "scene/Room.h"

namespace gamelib
{
	class player_component : public component
	{
	public:
		player_component(std::string component_name, std::shared_ptr<Player> player);

		std::shared_ptr<Player> the_player;
		
		int room;
	};
}
