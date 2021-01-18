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
	/**
	 * \brief Creates a new player
	 * \param x Player's x position
	 * \param y Player's y position
	 * \param w Player's width
	 * \param h Player's height
	 * \param settings The shared settings admin
	 * \param event_admin The shared event admin
	 */
	Player::Player(const int x, const int y, const int w, const int h,
	               const std::shared_ptr<settings_manager> settings, std::shared_ptr<event_manager> event_admin):
		DrawingBase(x, y, true, settings), event_admin(std::move(event_admin)), width(w), height(h)
	{
	}

	void Player::load_settings(std::shared_ptr<settings_manager> settings)
	{
		GameObject::load_settings(settings); // Call base
		
		draw_box = settings->get_bool("player","draw_box");
	}

	void Player::center_player_in_room(shared_ptr<Room> target_room)
	{
		/**
		 * Function to the center the player in the given room
		 */
		const std::function<coordinate<int>(Room, Player)> center_player = [](const Room& room, Player p)
		{
			auto const room_x_mid = room.get_x() + (room.get_w() / 2);
			auto const room_y_mid = room.get_y() + (room.get_h() / 2);
			auto const x = room_x_mid - p.width /2;
			auto const y = room_y_mid - p.height /2;			
			return coordinate<int>(x, y);
		};
		
		const auto coords = center_player(*target_room, *this);
		y = coords.get_y();
		x = coords.get_x();
		update();
	}

	
	int Player::get_room_neighbour_index(const int first_room_index, const int last_room_index, const int side, shared_ptr<Room> room)
	{
		switch(side)
		{
		case 0: /*top*/  // NOLINT(bugprone-branch-clone)
			return room->get_adjacent_index_for_wall(side) < first_room_index ? first_room_index : room->get_adjacent_index_for_wall(side);
		case 1:  /*right*/
			return room->get_adjacent_index_for_wall(side) < first_room_index ? first_room_index : room->get_adjacent_index_for_wall(side);	
		case 2:  /*bottom*/
			return room->get_adjacent_index_for_wall(side) > last_room_index ? last_room_index : room->get_adjacent_index_for_wall(side);
		case 3: /*left*/
			return room->get_adjacent_index_for_wall(side) < 0 ? 0 : room->get_adjacent_index_for_wall(side);
			default:
			return first_room_index;
		}		
	}

	vector<shared_ptr<event>> Player::handle_event(const std::shared_ptr<event> the_event)
	{
		vector<shared_ptr<event>> created_events;

		// Call base class
		for(auto &e : GameObject::handle_event(the_event)) 
			created_events.push_back(e);
		
		// Process Player events
		if(event_type::PositionChangeEventType == the_event->type)
		{
						
			const auto position_changed_event = dynamic_pointer_cast<position_change_event>(the_event);
			const auto move_direction = position_changed_event->direction;			
					
			auto game_world = static_pointer_cast<game_world_component>(find_component(constants::game_world))->get_data();
			
			const auto last_room_index = count_if(begin(game_world->game_objects), end(game_world->game_objects),
			                                      [](weak_ptr<GameObject> g){
					if(auto ptr = g.lock()){
						return ptr->get_type() == object_type::room;
					}
				});
			const auto first_room_index = 0;

			auto current_room = dynamic_pointer_cast<Room>(game_world->game_objects[within_room_index < first_room_index ? first_room_index : within_room_index]);			
			auto above_room = dynamic_pointer_cast<Room>(game_world->game_objects[get_room_neighbour_index(first_room_index, last_room_index, 0, current_room)]);
			auto right_room = dynamic_pointer_cast<Room>(game_world->game_objects[get_room_neighbour_index(first_room_index, last_room_index, 1, current_room)]);
			auto bottom_room = dynamic_pointer_cast<Room>(game_world->game_objects[get_room_neighbour_index(first_room_index, last_room_index, 2, current_room)]);
			auto left_room = dynamic_pointer_cast<Room>(game_world->game_objects[get_room_neighbour_index(first_room_index, last_room_index, 3, current_room)]);
			
			const auto is_moving_right = move_direction == Direction::Right;
			const auto is_moving_down = move_direction == Direction::Down;
			const auto is_moving_up = move_direction == Direction::Up;
			const auto is_moving_left = move_direction == Direction::Left;
			
			const auto can_move_right = is_moving_right && !current_room->is_walled_0_based(1) && !right_room->is_walled_0_based(3);
			const auto can_move_left = is_moving_left && !current_room->is_walled_0_based(3) && !left_room->is_walled_0_based(1);
			const auto can_move_down = is_moving_down && !current_room->is_walled_0_based(2) && !bottom_room->is_walled_0_based(0);
			const auto can_move_up = is_moving_up && !current_room->is_walled_0_based(0 )&& !above_room->is_walled_0_based(2);
			
			const auto is_valid_move = move_direction == Direction::Down && can_move_down ||
											   move_direction == Direction::Left && can_move_left || 
											   move_direction == Direction::Right && can_move_right || 
											   move_direction == Direction::Up && can_move_up;
			
			if(!is_valid_move){
				log_message("Invalid move", settings_admin->get_bool("player", "verbose"));
				event_admin->raise_event(make_shared<event>(event_type::InvalidMove), GameObject::downcasted_shared_from_this<Player>());				
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

			// Player moved, tell the world about my move!
			auto component = make_shared<player_component>(constants::playerComponentName, GameObject::downcasted_shared_from_this<Player>());	
			created_events.push_back(make_shared<player_moved_event>(component, move_direction));
		}

		if(event_type::DoLogicUpdateEventType  == the_event->type)
			update();

		return created_events;
	}

	

	void Player::draw(SDL_Renderer* renderer)
	{
		SDL_Rect r = { x ,y, width, height};
		
		SDL_Color c = {
			static_cast<Uint8>(settings_admin->get_int("player", "r")),
			static_cast<Uint8>(settings_admin->get_int("player", "g")),
			static_cast<Uint8>(settings_admin->get_int("player", "b")),
			static_cast<Uint8>(settings_admin->get_int("player", "a"))			
		};
		
		DrawFilledRect(renderer, &r, c);
	}

	string Player::get_identifier()
	{
		return "square";
	}

	void Player::update()
	{
		bounds = {x, y, width, height};
	}
}
