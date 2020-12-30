#include "Player.h"
#include "player_component.h"
#include <memory>


#include "game_world_component.h"
#include "common/Common.h"
#include "common/constants.h"
#include "events/player_moved_event.h"
using namespace std;

namespace gamelib
{
	player::player(int x, int y, int w,  std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings_admin): square(0, x, y, w, resource_admin, true, true, true, settings_admin)
	{
		
		const auto player = std::make_shared<player_component>(constants::playerComponentName, x, y, w, w);
		set_tag(constants::playerTag);
		add_component(player);		
	}

	void player::load_settings(std::shared_ptr<settings_manager> settings_admin)
	{
		game_object::load_settings(settings_admin);
		width = settings_admin->get_int("player","box_width");
		draw_box = settings_admin->get_bool("player","draw_box");
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
			auto game_world = static_pointer_cast<game_world_component>(find_component(constants::game_world))->get_data();
			auto room = dynamic_pointer_cast<square>(game_world->game_objects[player->room < 0 ? 0 : player->room]);
			const auto is_moving_right = player_bounds_.x <  get_x();
			const auto is_moving_down = player_bounds_.y < get_y();
			
			const auto can_move_right = is_moving_right && !room->is_walled_0_based(1);
			const auto can_move_left = !is_moving_right && !room->is_walled_0_based(3);
			const auto can_move_down = is_moving_down && !room->is_walled_0_based(2);
			const auto can_move_up = !is_moving_down && !room->is_walled_0_based(0);
			const auto valid_move = can_move_right || can_move_left || can_move_down || can_move_up;
			if(!valid_move){
				log_message("Invalid move", settings_admin->get_bool("player", "verbose"));
			}
			
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
		if(draw_box)
			SDL_RenderDrawRect(renderer, &player_bounds_);
	}

	string player::get_identifier()
	{
		return "square";
	}

}