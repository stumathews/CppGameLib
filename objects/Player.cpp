#include "Player.h"
#include "player_component.h"
#include <memory>

#include "common/constants.h"
#include "events/player_moved_event.h"
using namespace std;

namespace gamelib
{
	player::player(int x, int y, int w,  std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings_admin): square(x, y, w, resource_admin, true, true, true, settings_admin)
	{
		
		const auto player = std::make_shared<player_component>(constants::playerComponentName, x, y, w, w);
		set_tag(constants::playerTag);
		add_component(player);

		
	}

	vector<shared_ptr<event>> player::handle_event(const std::shared_ptr<event> the_event)
	{	
		// Process GameObject events
		auto created_events(game_object::handle_event(the_event));

		// Process Square events
		for(auto &e : square::handle_event(the_event)) 
			created_events.push_back(e);
		
		// Process Player events
		if(event_type::PositionChangeEventType == the_event->type)
		{
			auto player = static_pointer_cast<player_component>(find_component(constants::playerComponentName));
			
			player_bounds_.x = player->x = get_x();
			player_bounds_.y = player->y = get_y();
			player_bounds_.w = player->w = get_w();
			player_bounds_.h = player->h = get_h();

			created_events.push_back(make_shared<player_moved_event>(player));	
		}
		return created_events;
	}

	void player::draw(SDL_Renderer* renderer)
	{
	  // Let the player draw itself
		SDL_RenderDrawRect(renderer, &player_bounds_);
	}

	string player::get_identifier()
	{
		return "square";
	}

}