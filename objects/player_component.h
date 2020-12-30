#pragma once
#include "Component.h"
#include "Player.h"
#include "scene/Square.h"

namespace gamelib
{
	class player_component : public component
	{
	public:
		player_component(std::string component_name, std::shared_ptr<player> player);

		std::shared_ptr<player> the_player;
		int room;
	};
}
