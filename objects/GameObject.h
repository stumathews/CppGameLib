#pragma once

#include <map>
#include <SDL.h>
#include "Component.h"
#include <graphic/graphic_resource.h>
#include "events/EventManager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>

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

	class GameObject : public IEventSubscriber, public inheritable_enable_shared_from_this<IEventSubscriber>
	{
	public:
		int id;
		bool supports_move_logic;
		bool is_visible;
		bool is_color_key_enabled;
		int x;
		int y;
		int move_interval;		
		SDL_Rect bounds;
		SettingsManager& settings_admin;
		EventManager& eventManager;
		bool is_active = true;

		// Create new Game Object 
		GameObject(bool is_visible, SettingsManager& settings_admin, EventManager& eventManager);

		// Create new game object at specific coordinate
		GameObject(int x, int y, bool is_visible, SettingsManager& settings_admin, EventManager& eventManager);
		
		// Initialize game objects defaults
		void init_defaults(bool is_visible, SettingsManager& settings, int x, int y);
		
		
		/* IEventSubscriber methods; All Game Objects responds to events */
		void subscribe_to_event(event_type type, EventManager& event_admin);
		void raise_event(const std::shared_ptr<event>& the_event, EventManager& event_admin);
		std::vector<std::shared_ptr<event>> handle_event( std::shared_ptr<event> event) override;
		std::string get_subscriber_name() override;
		virtual int get_subscriber_id() override { return id; }
		
		// Get the Game Object's graphic if there is one associated with the game object
		std::shared_ptr<graphic_resource> get_graphic() const;

		// Set the Game Object's graphic
		void set_graphic(std::shared_ptr<graphic_resource> graphic);
		bool has_graphic() const;
		
		// Every Game Object can update itself itself diffirently
		void virtual update() = 0;

		// All game objects can be drawn uniformly
		void virtual draw(SDL_Renderer* renderer);
		void draw_resource(SDL_Renderer* renderer) const;

		// Every game Object needs to identify what type of game object it is
		virtual object_type get_type() = 0;

		// Every Game Object can move in its own way
		virtual void move_up();
		virtual void move_down();
		virtual void move_left();
		virtual void move_right();

		// Every game object can identify itself in its own way
		virtual std::string get_identifier();

		// Every game object can load its own settings
		virtual void load_settings(SettingsManager& settings_admin);
		
		// Every game Object can change it own internal position
		virtual void change_internal_position(std::shared_ptr<event> the_event);
		
		void set_color_key(Uint8 r, Uint8 g, Uint8 b);

		/* Inventory System: game objects can have a list of components */
		
		// Add component to the game object (inventory)
		void add_component(const std::shared_ptr<component>& component);
		std::shared_ptr<component> find_component(std::string name);
		bool has_component(std::string name);

		// Arbitary tag for game object
		std::string get_tag() const;
		void set_tag(std::string tag);
	private:

		// Game Object tag
		std::string tag;
		bool is_traveling_left;

		// Game Object colour
		int red, blue, green;
		SDL_Color color_key = {};

		// Game Object Graphic
		std::shared_ptr<graphic_resource> graphic; // can be shared by other actors
		
		// list of components
		std::map<std::string, std::shared_ptr<component>> components;
		
		// Underlying asset
		std::shared_ptr<asset> underlying_asset;

		// All Game Object has ids
		static int ids;
	};
}



