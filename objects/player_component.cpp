#include "player_component.h"

gamelib::player_component::player_component(std::string component_name, Player* player)
	: component(component_name), the_player(player), room(0)
{
	// init members only
}
