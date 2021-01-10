#include "Square.h"
#include "util/RectDebugging.h"
#include <events/player_moved_event.h>


using namespace std;

namespace gamelib
{
	square::~square() = default;
		
	vector<shared_ptr<event>> square::handle_event(const std::shared_ptr<event> event)
	{	
		auto any_generated_events(game_object::handle_event(event));

		if(event->type == event_type::PlayerMovedEventType)
		{			
			const auto moved_event = std::static_pointer_cast<player_moved_event>(event);				
			const auto player_component = moved_event->get_player_component();
			const auto player = player_component->the_player;

			if(get_type() == object_type::player)
				return any_generated_events;

			// Determine if the player's coordinates are within room
			is_player_in_square = (player->x >= bounds.x && player->x < bounds.x + bounds.w) && (player->y >= bounds.y && player->y < bounds.y + bounds.h);
			
			// Update the player's knowledge of which room its in
			if(is_player_in_square)
				player->within_room_index = number;

			// Update square's knowledge of player's bounds for layer use
			player_bounds = { player->get_x(), player->get_y(), player->get_w(), player->get_h()  };
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

	void square::load_settings(std::shared_ptr<settings_manager> settings_admin)
	{
		game_object::load_settings(settings_admin);		
		fill = settings_admin->get_bool("room_fill", "enable");
	}

	void square::fill_me(SDL_Renderer* renderer, const SDL_Color color) const
	{			
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &bounds);
	}

	void square::draw(SDL_Renderer* renderer)
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
			fill_me(renderer, { 255, 0 ,0 ,0});
		}
		

		if(settings_admin->get_bool("global", "print_debugging_text"))
		  RectDebugging::printInRect(renderer, get_tag(), &bounds, resource_admin); 
	}

	shared_ptr<abcd_rectangle> square::get_abcd() const
	{
		return abcd;
	}

	square::square(int number, int x, int y, int rw, int rh, const std::shared_ptr<resource_manager> resource_admin,
	               bool fill, const bool supports_move_logic, const bool is_visible,
	               const std::shared_ptr<settings_manager> settings_admin, square_role role)
		: game_object(x, y, is_visible, settings_admin), resource_admin(resource_admin), fill(fill), player_bounds({}), role(role),
		  width(rw),
		  height(rh), bounds({x, y, rw, rh}), top_room_index(0), right_room_index(0),
		  bottom_room_index(0),
		  settings_admin(settings_admin)
	{
		this->number = number;
		this->abcd = make_shared<abcd_rectangle>(x, y, rw, rh);
		this->supports_move_logic = supports_move_logic;
		walls[0] = true;
		walls[1] = true;
		walls[2] = true;
		walls[3] = true;
	}

	void square::set_adjacent_room_index(const int top_index, const int right_index, const int bottom_index, const int left_index)
	{
		this->top_room_index = top_index;
		this->right_room_index = right_index;
		this->bottom_room_index = bottom_index;
		this->left_room_index = left_index;
	}

	int square::get_adjacent_index_for_wall(int index) const
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

	int square::get_x() const
	{
		return this->x;
	}

	int square::get_y() const
	{
		return this->y;
	}

	int square::get_w() const
	{
		return width;
	}

	int square::get_h() const
	{
		return height;
	}

	bool square::is_walled(int wall)
	{
		return walls[wall-1];
	}

	bool square::is_walled_0_based(int wall)
	{
		return walls[wall];
	}

	void square::update()
	{
	}

	void square::remove_wall(int wall)
	{
		this->walls[wall - 1] = false;
	}

	string square::get_identifier()
	{
		return "square";
	}

}




