#include "game_object.h"
#include <memory>
#include <iostream>
#include <SDL_mixer.h>


#include "audio/AudioResource.h"
#include "common/constants.h"
#include "events/event_manager.h"
#include "events/PositionChangeEvent.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/resource_manager.h"
using namespace std;

namespace gamelib
{

	void game_object::change_object_position(const std::shared_ptr<event> the_event)
	{
		const auto event = std::dynamic_pointer_cast<position_change_event>(the_event);
		if(event->direction == Direction::Up && supports_move_logic)					
			move_up();			
			
		if(event->direction == Direction::Down && supports_move_logic)
			move_down();			
			
		if(event->direction == Direction::Left && supports_move_logic)
			move_left();			
			
		if(event->direction == Direction::Right && supports_move_logic)
			move_right();
	}

	vector<shared_ptr<event>> game_object::handle_event(const std::shared_ptr<event> the_event)
	{
		switch(the_event->type)  // NOLINT(clang-diagnostic-switch-enum)
		{
			case event_type::PositionChangeEventType:
				change_object_position(the_event);	
				break;
			case event_type::DoLogicUpdateEventType:
				update();
				break;
			default: break ;  // NOLINT(clang-diagnostic-covered-switch-default)
		}
		return vector<shared_ptr<event>>();
	}

	void game_object::subscribe_to_event(event_type type, shared_ptr<event_manager> event_admin)
	{
		event_admin->subscribe_to_event(type, this);
	}

	void game_object::raise_event(const shared_ptr<event>& the_event, shared_ptr<event_manager> event_admin)
	{
		event_admin->raise_event(the_event, this);
	}

	shared_ptr<graphic_resource> game_object::get_graphic_asset() const
	{
		return graphic;
	}

	void game_object::draw(SDL_Renderer * renderer)
	{
		if(!is_visible)
			return;
		
		draw_resource(renderer);
	}

	void game_object::move_up()
	{
		y -= move_interval;
	}

	void game_object::move_down()
	{
		y += move_interval;
	}

	void game_object::move_left()
	{
		x -= move_interval;
	}

	void game_object::move_right()
	{
		x += move_interval;
	}

	string game_object::get_identifier()
	{
		return "game_object";
	}

	/*void game_object::detect_side_collision(shared_ptr<sdl_graphics_manager> graphics_admin, shared_ptr<resource_manager> resource_admin)
	{
		if (is_traveling_left)
		{
			if (x == 0) 
			{
				is_traveling_left = false;
				Mix_PlayChannel(-1, static_pointer_cast<audio_resource>(resource_admin->get("high.wav"))->as_fx(), 0);
			}
		}
		else
		{
			if (x == graphics_admin->get_screen_width())
			{
				Mix_PlayChannel(-1, static_pointer_cast<audio_resource>(resource_admin->get("low.wav"))->as_fx(), 0);
				is_traveling_left = true;
			}
		}
	}*/

	game_object::game_object(bool is_visible): event_subscriber(), supports_move_logic(true), is_visible(is_visible), is_color_key_enabled(false), x(0), y(0), is_traveling_left(false), red(0x00), blue(0xFF), green(0x00)
	{	
	}

	game_object::game_object(const int x, const int y, bool is_visible): event_subscriber(), supports_move_logic(false), is_visible(is_visible), x(x), y(y)
	{
		red = 0x00;
		blue = 0xFF;
		green = 0x00;
		is_traveling_left = false;
		is_visible = true;
		is_color_key_enabled = false;
	}


	void game_object::set_color_key(const Uint8 r, const Uint8 g, const Uint8 b)
	{
		color_key.r = r;
		color_key.g = g;
		color_key.b = b;
	}

	void game_object::add_component(const shared_ptr<component>& component)
	{
		components[component->get_name()] = component;
	}

	bool game_object::is_player()
	{
		return find_component(constants::playerComponentName) != nullptr;
	}

	shared_ptr<component> game_object::find_component(string name)
	{
		return components[name];
	}

	bool game_object::has_component(string name)
	{
		return components.find(name) != components.end();
	}

	void game_object::set_tag(const string tag)
	{
		this->tag = tag;
	}

	string game_object::get_subscriber_name()
	{
		return "game_object";
	}


	void game_object::draw_resource(SDL_Renderer* renderer) const
	{
		const auto resource = get_graphic_asset();
		if(resource != nullptr && resource->type == "graphic")
		{
			SDL_Rect draw_location = { x, y, 100,100 };
			auto* const rect = get_graphic_asset()->is_animated
				                   ?  &graphic->view_port
				                   : nullptr;
			SDL_RenderCopy(renderer, graphic->texture, rect, &draw_location );
		}
	}

	bool game_object::is_resource_loaded() const
	{
		return graphic != nullptr;
	}



	string game_object::get_tag() const
	{
		return this->tag;
	}

	void game_object::set_graphic_resource(shared_ptr<graphic_resource> graphic)
	{
		this->graphic = graphic;
	}
}