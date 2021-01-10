#include "Room.h"
#include "util/RectDebugging.h"
#include <events/player_moved_event.h>


using namespace std;

namespace gamelib
{
	Room::~Room() = default;
		
	vector<shared_ptr<event>> Room::handle_event(const std::shared_ptr<event> event)
	{	
		auto any_generated_events(GameObject::handle_event(event));

		if(event->type == event_type::PlayerMovedEventType)
		{			
			const auto moved_event = std::static_pointer_cast<player_moved_event>(event);				
			const auto player_component = moved_event->get_player_component();
			const auto player = player_component->the_player;

			if(get_type() == object_type::player)
				return any_generated_events;

			// Determine if the player's coordinates are within room
			is_player_in_room = (player->x >= bounds.x && player->x < bounds.x + bounds.w) && (player->y >= bounds.y && player->y < bounds.y + bounds.h);
			
			// Update the player's knowledge of which room its in
			if(is_player_in_room)
				player->within_room_index = number;

			// Update square's knowledge of player's bounds for layer use
			player_bounds = { player->x, player->y, player->width, player->height  };
		}

		if( event->type == event_type::SettingsReloaded)
		{
			load_settings(settings_admin);			
		}
		if(event->type == event_type::Fire)
		{
			
		}
		
		return any_generated_events;
	}

	void Room::load_settings(std::shared_ptr<settings_manager> settings_admin)
	{
		GameObject::load_settings(settings_admin);		
		fill = settings_admin->get_bool("room_fill", "enable");
	}

	void Room::draw(SDL_Renderer* renderer)
	{
		// black
		SDL_SetRenderDrawColor(renderer, 0, 0,0,0);
		
		const auto rect = get_abcd();
		const auto ax = rect->get_ax();
		const auto ay = rect->get_ay();
		const auto bx = rect->get_bx();
		const auto by = rect->get_by();
		const auto cx = rect->get_cx();
		const auto cy = rect->get_cy();
		const auto dx = rect->get_dx();
		const auto dy = rect->get_dy();

		const auto have_top_wall = this->walls[0];
		const auto have_right_wall = this->walls[1];
		const auto have_bottom_wall = this->walls[2];
		const auto have_left_wall = this->walls[3];

		if (have_top_wall) 
			SDL_RenderDrawLine(renderer, ax,ay,bx,by);
		
		if (have_right_wall) 
			SDL_RenderDrawLine(renderer, bx,by,cx,cy);
		
		if (have_bottom_wall) 
			SDL_RenderDrawLine(renderer, cx,cy,dx,dy);
		
		if (have_left_wall) 
			SDL_RenderDrawLine(renderer, dx,dy,ax,ay);
				
		if(fill)
		{
			DrawFilledRect(renderer, &bounds, { 255, 0 ,0 ,0});
		}
		

		if(settings_admin->get_bool("global", "print_debugging_text"))
		  RectDebugging::printInRect(renderer, get_tag(), &bounds, resource_admin); 
	}

	shared_ptr<abcd_rectangle> Room::get_abcd() const
	{
		return abcd;
	}

	Room::Room(int number, int x, int y, int rw, int rh, const std::shared_ptr<resource_manager> resource_admin, bool fill, const std::shared_ptr<settings_manager> settings)
		: DrawingBase(x, y, true, settings), resource_admin(resource_admin), fill(fill), player_bounds({}),
		 top_room_index(0), right_room_index(0),  bottom_room_index(0), width(rw), height(rh)
	{
		this->bounds = {x, y, rw, rh};
		this->width = rw;
		this->height = rh;
		this->number = number;
		this->abcd = make_shared<abcd_rectangle>(x, y, rw, rh);
		this->supports_move_logic = false;
		walls[0] = true;
		walls[1] = true;
		walls[2] = true;
		walls[3] = true;
	}

	void Room::set_adjacent_room_index(const int top_index, const int right_index, const int bottom_index, const int left_index)
	{
		this->top_room_index = top_index;
		this->right_room_index = right_index;
		this->bottom_room_index = bottom_index;
		this->left_room_index = left_index;
	}

	int Room::get_adjacent_index_for_wall(int index) const
	{
		switch (index)
		{
		case 0:
			return top_room_index;
			break;
		case 1:
			return right_room_index;
			break;
		case 2:
			return bottom_room_index;
			break;
		case 3:
			return left_room_index;
			break;
		default:
			return -1;
		}
	}

	int Room::get_x() const
	{
		return this->x;
	}

	int Room::get_y() const
	{
		return this->y;
	}

	int Room::get_w() const
	{
		return width;
	}

	int Room::get_h() const
	{
		return height;
	}

	bool Room::is_walled(int wall)
	{
		return walls[wall-1];
	}

	bool Room::is_walled_0_based(int wall)
	{
		return walls[wall];
	}

	void Room::update()
	{
	}

	void Room::remove_wall(int wall)
	{
		this->walls[wall - 1] = false;
	}

	string Room::get_identifier()
	{
		return "square";
	}

}




