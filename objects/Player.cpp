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
	               SettingsManager& settings, EventManager& eventManager, 
		           Logger& gameLogger):
		DrawableGameObject(x, y, true, settings, eventManager), eventManager(eventManager), width(w), height(h), gameLogger(gameLogger)
	{
		be_verbose = settings_admin.get_bool("player", "verbose");
	}

	void Player::LoadSettings(SettingsManager& settings)
	{
		GameObject::LoadSettings(settings); // Call base
		
		drawBox = settings.get_bool("player", "draw_box");
	}

	void Player::CenterPlayerInRoom(shared_ptr<Room> target_room)
	{
		/**
		 * Function to the center the player in the given room
		 */
		const std::function<coordinate<int>(Room, Player)> center_player = [](const Room& room, Player p)
		{
			auto const room_x_mid = room.GetX() + (room.GetWidth() / 2);
			auto const room_y_mid = room.GetY() + (room.GetHeight() / 2);
			auto const x = room_x_mid - p.width /2;
			auto const y = room_y_mid - p.height /2;			
			return coordinate<int>(x, y);
		};
		
		const auto coords = center_player(*target_room, *this);
		y = coords.GetY();
		x = coords.GetX();
		Update();
	}

	
	int Player::get_room_neighbour_index(const int first_room_index, const int last_room_index, const int side, shared_ptr<Room> room)
	{
		
		switch(side)
		{
		case 0: /*top*/  // NOLINT(bugprone-branch-clone)
			return room->GetNeighbourIndex((Side)side) < first_room_index ? first_room_index : room->GetNeighbourIndex((Side)side);
		case 1:  /*right*/
			return room->GetNeighbourIndex((Side)side) < first_room_index ? first_room_index : room->GetNeighbourIndex((Side)side);	
		case 2:  /*bottom*/
			return room->GetNeighbourIndex((Side)side) > last_room_index ? last_room_index : room->GetNeighbourIndex((Side)side);
		case 3: /*left*/
			return room->GetNeighbourIndex((Side)side) < 0 ? 0 : room->GetNeighbourIndex((Side)side);
			default:
			return first_room_index;
		}		
	}

	vector<shared_ptr<Event>> Player::HandleEvent(const std::shared_ptr<Event> the_event)
	{
		vector<shared_ptr<Event>> created_events;

		// Call base class
		for(auto &e : GameObject::HandleEvent(the_event)) 
			created_events.push_back(e);
		
		// Process Player events
		if(EventType::PositionChangeEventType == the_event->type)
		{
						
			const auto position_changed_event = dynamic_pointer_cast<position_change_event>(the_event);
			const auto move_direction = position_changed_event->direction;			
					
			auto game_world = static_pointer_cast<game_world_component>(FindComponent(constants::game_world))->get_data();
			
			const auto last_room_index = count_if(begin(game_world.objects), end(game_world.objects),
			                                      [](weak_ptr<GameObject> g){
					if(auto ptr = g.lock()){
						return ptr->GetGameObjectType() == object_type::Room;
					}
					return false;
				});
			const auto first_room_index = 0;

			auto current_room = dynamic_pointer_cast<Room>(game_world.objects[within_room_index < first_room_index ? first_room_index : within_room_index]);			
			auto above_room = dynamic_pointer_cast<Room>(game_world.objects[get_room_neighbour_index(first_room_index, last_room_index, 0, current_room)]);
			auto right_room = dynamic_pointer_cast<Room>(game_world.objects[get_room_neighbour_index(first_room_index, last_room_index, 1, current_room)]);
			auto bottom_room = dynamic_pointer_cast<Room>(game_world.objects[get_room_neighbour_index(first_room_index, last_room_index, 2, current_room)]);
			auto left_room = dynamic_pointer_cast<Room>(game_world.objects[get_room_neighbour_index(first_room_index, last_room_index, 3, current_room)]);
			
			const auto is_moving_right = move_direction == Direction::Right;
			const auto is_moving_down = move_direction == Direction::Down;
			const auto is_moving_up = move_direction == Direction::Up;
			const auto is_moving_left = move_direction == Direction::Left;
			
			const auto can_move_right = is_moving_right && !current_room->IsWalled(Side::Right) && !right_room->IsWalled(Side::Left);
			const auto can_move_left = is_moving_left && !current_room->IsWalled(Side::Left) && !left_room->IsWalled(Side::Right);
			const auto can_move_down = is_moving_down && !current_room->IsWalled(Side::Bottom) && !bottom_room->IsWalled(Side::Top);
			const auto can_move_up = is_moving_up && !current_room->IsWalled(Side::Top)&& !above_room->IsWalled(Side::Bottom);
			
			const auto is_valid_move = move_direction == Direction::Down && can_move_down ||
											   move_direction == Direction::Left && can_move_left || 
											   move_direction == Direction::Right && can_move_right || 
											   move_direction == Direction::Up && can_move_up;
			
			// An inavalid move is when we want to move in a direction but we cant
			if(!is_valid_move)
			{
				LogMessage("Invalid move", gameLogger, be_verbose);
				eventManager.RaiseEvent(make_shared<Event>(EventType::InvalidMove), this);				
				return created_events;
			}
			
			// snap to room
			if(is_moving_down) 
				CenterPlayerInRoom(bottom_room);			
			
			if(is_moving_up) 
				CenterPlayerInRoom(above_room);				
			
			if(is_moving_right)
				CenterPlayerInRoom(right_room);
			
			if(is_moving_left)
				CenterPlayerInRoom(left_room);

			// Player moved, tell the world about my move!
			auto component = std::shared_ptr<player_component>(new player_component(constants::playerComponentName, this));	
			created_events.push_back(make_shared<player_moved_event>(component, move_direction));
		}

		if(EventType::DoLogicUpdateEventType  == the_event->type)
		{
			Update();
		}

		return created_events;
	}

	

	void Player::Draw(SDL_Renderer* renderer)
	{
		SDL_Rect r = { x ,y, width, height};
		
		SDL_Color c = {
			static_cast<Uint8>(settings_admin.get_int("player", "r")),
			static_cast<Uint8>(settings_admin.get_int("player", "g")),
			static_cast<Uint8>(settings_admin.get_int("player", "b")),
			static_cast<Uint8>(settings_admin.get_int("player", "a"))			
		};
		
		DrawFilledRect(renderer, &r, c);
	}

	string Player::GetName()
	{
		return "player";
	}

	object_type Player::GetGameObjectType() { return object_type::Player; }

	void Player::Update()
	{
		bounds = 
		{
			x,
			y,
			width,
			height
		};
	}

	void Player::SetRoom(int roomIndex) 
	{ 
		within_room_index = roomIndex; 
	}

	int Player::GetWidth() 
	{ 
		return width; 
	}
	
	int Player::GetHeight()
	{
		return height;
	}
}
