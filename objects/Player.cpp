#include "Player.h"
#include "player_component.h"
#include <memory>


#include "game_world_component.h"
#include "common/Common.h"
#include "common/constants.h"
#include "events/player_moved_event.h"
#include "events/PositionChangeEvent.h"
#include <functional>

using namespace std;

namespace gamelib
{
	player::player(int x, int y, int w, int h, std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings, std::shared_ptr<event_manager> event_admin):
		square(0, x, y, w, h, resource_admin, true, true, true, settings), draw_box(false),
		event_admin(std::move(event_admin))
	{
	}

	void player::load_settings(std::shared_ptr<settings_manager> settings)
	{
		// Call base
		square::load_settings(settings);
		x = settings->get_int("player","x");
		y = settings->get_int("player","y");
		
		draw_box = settings->get_bool("player","draw_box");
	}

	void player::center_player_in_room(shared_ptr<square> target_room)
	{
		const std::function<coordinate<int>(square, player)> center_player = [](square room, player p)
		{
			auto const room_x_mid = room.get_x() + (room.get_w() / 2);
			auto const room_y_mid = room.get_y() + (room.get_h() / 2);
			auto const x = room_x_mid - p.get_w() /2;
			auto const y = room_y_mid - p.get_h() /2;
			
			return coordinate<int>(x, y);
		};
		
		const auto coords = center_player(*target_room, *this);
		y = coords.get_y();
		x = coords.get_x();
	}

	vector<shared_ptr<event>> player::handle_event(const std::shared_ptr<event> the_event)
	{
		vector<shared_ptr<event>> created_events;

		// Call base class
		for(auto &e : square::handle_event(the_event)) 
			created_events.push_back(e);
		
		// Process Player events
		if(event_type::PositionChangeEventType == the_event->type)
		{
						
			const auto position_changed_event = dynamic_pointer_cast<position_change_event>(the_event);
			const auto move_direction = position_changed_event->direction;
			
			auto component = make_shared<player_component>(constants::playerComponentName, shared_from_this());
			
			auto game_world = static_pointer_cast<game_world_component>(find_component(constants::game_world))->get_data();

			const auto last_room_index = game_world->game_objects.size() -1; /* discount player object */
			const auto first_room_index = 0;

			auto room = dynamic_pointer_cast<square>(game_world->game_objects[within_room_index < first_room_index ? first_room_index : within_room_index]);
			
			const auto room_above_index = room->get_adjacent_index_for_wall(0) < first_room_index ? first_room_index : room->get_adjacent_index_for_wall(0);
			const auto room_right_index = room->get_adjacent_index_for_wall(1) < first_room_index ? first_room_index : room->get_adjacent_index_for_wall(1);			
			const auto room_bottom_index = room->get_adjacent_index_for_wall(2) > last_room_index ? last_room_index : room->get_adjacent_index_for_wall(2);
			const auto room_left_index = room->get_adjacent_index_for_wall(3) < 0 ? 0 : room->get_adjacent_index_for_wall(3);
			
			auto above_room = dynamic_pointer_cast<square>(game_world->game_objects[room_above_index]);
			auto right_room = dynamic_pointer_cast<square>(game_world->game_objects[room_right_index]);
			auto bottom_room = dynamic_pointer_cast<square>(game_world->game_objects[room_bottom_index]);
			auto left_room = dynamic_pointer_cast<square>(game_world->game_objects[room_left_index]);
			
			const auto is_moving_right = move_direction == Direction::Right;
			const auto is_moving_down = move_direction == Direction::Down;
			const auto is_moving_up = move_direction == Direction::Up;
			const auto is_moving_left = move_direction == Direction::Left;
			
			const auto can_move_right = is_moving_right && !room->is_walled_0_based(1) && !right_room->is_walled_0_based(3);
			const auto can_move_left = is_moving_left && !room->is_walled_0_based(3) && !left_room->is_walled_0_based(1);
			const auto can_move_down = is_moving_down && !room->is_walled_0_based(2) && !bottom_room->is_walled_0_based(0);
			const auto can_move_up = is_moving_up && !room->is_walled_0_based(0 )&& !above_room->is_walled_0_based(2);
			
			const auto is_valid_move = move_direction == Direction::Down && can_move_down || move_direction == Direction::Left && can_move_left || move_direction == Direction::Right && can_move_right || move_direction == Direction::Up && can_move_up;
			
			if(!is_valid_move){
				log_message("Invalid move", settings_admin->get_bool("player", "verbose"));
				event_admin->raise_event(make_shared<event>(event_type::InvalidMove), this);
				
				return created_events;
			}

			

			if(is_moving_down) 
				center_player_in_room(bottom_room);			
			
			if(is_moving_up) 
				center_player_in_room(above_room);			
			
			
			if(is_moving_right)
				center_player_in_room(right_room);
			
			if(is_moving_left)
				center_player_in_room(left_room);
			
			// Now that the internal position of the square has changed (see game_object), update position structures
			abcd->reinitialize(get_x(), get_y(), get_w(), get_h());
			bounds = { get_x(), get_y(), get_w(), get_h() };
									
			created_events.push_back(make_shared<player_moved_event>(component));
		}

		if(event_type::DoLogicUpdateEventType  == the_event->type)
		{
				update();
		}
		
		return created_events;
	}

	void player::draw(SDL_Renderer* renderer)
	{
		square::draw(renderer);
		
		fill_me(renderer);
	}

	string player::get_identifier()
	{
		return "square";
	}

}