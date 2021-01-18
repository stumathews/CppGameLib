#include "GameObject.h"
#include <memory>
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
	int GameObject::ids = 0;
	void GameObject::change_internal_position(const std::shared_ptr<event> the_event)
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

	vector<shared_ptr<event>> GameObject::handle_event(const std::shared_ptr<event> the_event)
	{
		// We dont handle any events
		return vector<shared_ptr<event>>();
	}

	void GameObject::subscribe_to_event(event_type type, shared_ptr<event_manager> event_admin)
	{
		event_admin->subscribe_to_event(type, shared_from_this());
	}

	void GameObject::raise_event(const shared_ptr<event>& the_event, shared_ptr<event_manager> event_admin)
	{
		event_admin->raise_event(the_event, shared_from_this());
	}

	shared_ptr<graphic_resource> GameObject::get_graphic_asset() const
	{
		return graphic;
	}

	void GameObject::draw(SDL_Renderer * renderer)
	{
		if(!is_visible)
			return;
		
		draw_resource(renderer);
	}

	void GameObject::move_up()
	{
		y -= move_interval;
	}

	void GameObject::move_down()
	{		
		y += move_interval;
	}

	void GameObject::move_left()
	{
		x -= move_interval;
	}

	void GameObject::move_right()
	{
		x += move_interval;
	}

	string GameObject::get_identifier()
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

	void GameObject::load_settings(std::shared_ptr<settings_manager> settings_admin)
	{
		move_interval = settings_admin->get_int("player","move_interval");
	}

	void GameObject::init_defaults( bool is_visible, shared_ptr<settings_manager> settings, int x, int y)
	{
		 this->bounds = { x, y, 0 ,0};
		 supports_move_logic = true;
		 this->is_visible = is_visible;
		 is_color_key_enabled = false;
		 this->x = x;
		 this->y = y;
		 is_traveling_left = false;
		 red = 0x00;
		 blue = 0xFF;
		 green = 0x00;
		 settings_admin = settings;
		 id = ids++;
	}

	GameObject::GameObject(bool is_visible, std::shared_ptr<settings_manager> settings_admin): IEventSubscriber()
	{
		init_defaults(is_visible, settings_admin, 0, 0);
	}

	GameObject::GameObject(const int x, const int y, bool is_visible, std::shared_ptr<settings_manager> settings_admin): IEventSubscriber(),
	supports_move_logic(false)
	{
		init_defaults(is_visible, settings_admin, x, y);
		GameObject::load_settings(settings_admin);
	}


	void GameObject::set_color_key(const Uint8 r, const Uint8 g, const Uint8 b)
	{
		color_key.r = r;
		color_key.g = g;
		color_key.b = b;
	}

	void GameObject::add_component(const shared_ptr<component>& component)
	{
		components[component->get_name()] = component;
	}

	shared_ptr<component> GameObject::find_component(string name)
	{
		return components[name];
	}

	bool GameObject::has_component(string name)
	{
		return components.find(name) != components.end();
	}

	void GameObject::set_tag(const string tag)
	{
		this->tag = tag;
	}

	string GameObject::get_subscriber_name()
	{
		return "game_object";
	}


	void GameObject::draw_resource(SDL_Renderer* renderer) const
	{
		const auto resource = get_graphic_asset();
		if(resource != nullptr && resource->type == "graphic")
		{
			SDL_Rect draw_location = { x, y, 50,50 };
			auto* const rect = get_graphic_asset()->is_animated
				                   ?  &graphic->view_port
				                   : nullptr;
			SDL_RenderCopy(renderer, graphic->texture, rect, &draw_location );
		}
	}

	bool GameObject::is_resource_loaded() const
	{
		return graphic != nullptr;
	}



	string GameObject::get_tag() const
	{
		return this->tag;
	}

	void GameObject::set_graphic_resource(shared_ptr<graphic_resource> graphic)
	{
		this->graphic = graphic;
	}
}