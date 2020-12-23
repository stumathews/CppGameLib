#pragma once
#include "common/Common.h"
#include "events/event_subscriber.h"
#include "objects/Player.h"
#include "scene/scene_manager.h"

namespace gamelib
{
	/***
	 * logical structure of the game such as the game initialization, game loop etc.
	 */
	class game_structure final : public event_subscriber
	{
		std::shared_ptr<event_manager> event_admin;
		std::shared_ptr<resource_manager> resource_admin;
		std::shared_ptr<settings_manager> settings_admin;
		std::shared_ptr<game_world_data> game_world;
		std::shared_ptr<scene_manager> scene_admin;
		std::shared_ptr<sdl_graphics_manager> graphics_admin;
	public:
		game_structure(std::shared_ptr<event_manager> event_admin, std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> config, std::shared_ptr<game_world_data> game_world, std::shared_ptr<scene_manager> scene_admin, std::shared_ptr<sdl_graphics_manager> graphics_admin);
		virtual ~game_structure() override;
		void init_game_world_data() const;
		bool initialize_sdl(int screen_width, int screen_height);		
		std::shared_ptr<player> create_player() const;
		void setup_player() const;
		bool initialize();
		bool game_loop();
		void update();
		void world_update() const;
		void player_update();
		void get_input();		
		void draw(float);
		bool load_content() const;
		static long get_tick_now();
		void spare_time(long) const;
		bool unload();	 // releases resources

		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> the_event) override;
		std::string get_subscriber_name() override;
	};
}
