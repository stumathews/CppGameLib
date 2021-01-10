#pragma once

#include <map>
#include <SDL.h>
#include "Component.h"
#include <graphic/graphic_resource.h>

#include "events/event_manager.h"

namespace gamelib
{
	enum class object_type
	{
		undefined,
		game_world,
		room,
		player,
		sprite,
		pickup,
	};

	class GameObject : public IEventSubscriber
	{
	public:
		bool supports_move_logic;
		bool is_visible;
		bool is_color_key_enabled;
		int x;
		int y;
		int move_interval;		
		SDL_Rect bounds;
		std::shared_ptr<settings_manager> settings_admin;
		
		void init_defaults(bool is_visible, std::shared_ptr<settings_manager> settings, int x, int y);
		
		GameObject(bool is_visible, std::shared_ptr<settings_manager> settings_admin);
		GameObject(int x, int y, bool is_visible, std::shared_ptr<settings_manager> settings_admin);

		void subscribe_to_event(event_type type, std::shared_ptr<event_manager> event_admin);
		void raise_event(const std::shared_ptr<event>& the_event, std::shared_ptr<event_manager> event_admin);
		std::shared_ptr<graphic_resource> get_graphic_asset() const;
		void set_graphic_resource(std::shared_ptr<graphic_resource> graphic);

		
		void virtual update() = 0;
		virtual void move_up();
		virtual void move_down();
		virtual void move_left();
		virtual void move_right();
		virtual std::string get_identifier();
		virtual void load_settings(std::shared_ptr<settings_manager> settings_admin);
		virtual object_type get_type() = 0;

		virtual void change_internal_position(std::shared_ptr<event> the_event);
		virtual std::vector<std::shared_ptr<event>> handle_event(const std::shared_ptr<event> event) override;  // NOLINT(readability-inconsistent-declaration-parameter-name)
		//void detect_side_collision(std::shared_ptr<sdl_graphics_manager> graphics_admin,  std::shared_ptr<resource_manager> resource_admin);
		void set_color_key(Uint8 r, Uint8 g, Uint8 b);

		void add_component(const std::shared_ptr<component>& component);

		std::shared_ptr<component> find_component(std::string name);
		bool has_component(std::string name);
		std::string get_tag() const;
		void set_tag(std::string tag);
		std::string get_subscriber_name() override;
		void draw_resource(SDL_Renderer* renderer) const;
		void virtual draw(SDL_Renderer* renderer);
		bool is_resource_loaded() const;
	private:
		int id;
		std::string tag;
		bool is_traveling_left;
		int red, blue, green;
		std::shared_ptr<graphic_resource> graphic; // can be shared by other actors
		std::map<std::string, std::shared_ptr<component>> components;
		SDL_Color color_key = {};
		std::shared_ptr<asset> underlying_asset;

	};
}



