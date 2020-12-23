#include "Square.h"
#include "util/RectDebugging.h"
#include <events/player_moved_event.h>


using namespace std;

namespace gamelib
{
	square::~square() = default;

	vector<shared_ptr<event>> square::handle_event(const std::shared_ptr<event> event)
	{	
		auto new_events(game_object::handle_event(event));  // Moves the square, if its set to be movable

		if(event->type == event_type::PlayerMovedEventType)
		{
			const auto moved_event = std::static_pointer_cast<player_moved_event>(event);
			const auto player = moved_event->get_player_component();
			// determine where the player is
			player_bounds_ = 	
			{ 
				player->x, 
				player->y, 
				player->w, 	
				player->h 
			};
		}

		if(event->type == event_type::PositionChangeEventType)
		{
			rect_details_->init(get_x(), get_y(), get_w(), get_h());
			my_bounds_ = { get_x(), get_y(), get_w(), get_h() };
		}
		
		return new_events;
	}

	void square::draw(SDL_Renderer* renderer)
	{
		// black
		SDL_SetRenderDrawColor(renderer, 0, 0,0,0);
		
		const auto rect = get_rect_details();
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

		if(fill & SDL_HasIntersection(&my_bounds_, &player_bounds_))
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderFillRect(renderer, &my_bounds_);
		}

		if(settings_admin->get_bool("global", "print_debugging_text"))
		  RectDebugging::printInRect(renderer, get_tag().c_str(), &my_bounds_, resource_admin); 
	}

	shared_ptr<rect_details> square::get_rect_details() const
	{
		return rect_details_;
	}

	square::square(int x, int y, int rw, std::shared_ptr<resource_manager> resource_admin, bool fill,  bool supports_move_logic, bool is_visible, std::shared_ptr<settings_manager> settings_admin)
	: game_object(x, y), width(rw), fill(fill), player_bounds_({}), my_bounds_({x, y, rw, rw}), resource_admin(resource_admin), settings_admin(std::move(settings_admin))
{
	  this->rect_details_ = make_shared<rect_details>(x, y, rw, rw);  
	  this->supports_move_logic = supports_move_logic;   
	  walls[0] = true;
	  walls[1] = true;
	  walls[2] = true;
	  walls[3] = true;  
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
		return width;
	}

	bool square::is_walled(int wall)
	{
		return walls[wall - 1];
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




