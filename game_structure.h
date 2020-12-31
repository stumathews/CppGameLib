#pragma once
#include "common/Common.h"
#include "events/event_subscriber.h"
#include "objects/game_world_data.h"
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
		std::shared_ptr<scene_manager> scene_admin;
		std::shared_ptr<sdl_graphics_manager> graphics_admin;
		std::shared_ptr<game_world_data> world;
		std::shared_ptr<audio_manager> audio_admin;
		std::function<void()> get_input_func;
	public:
		game_structure(std::shared_ptr<event_manager> event_admin, std::shared_ptr<resource_manager> resource_admin,
		               std::shared_ptr<settings_manager> config, std::shared_ptr<game_world_data> world,
		               std::shared_ptr<scene_manager> scene_admin, std::shared_ptr<sdl_graphics_manager> graphics_admin,
		               std::shared_ptr<audio_manager> audio_admin, std::function<void()> get_input_func);
		virtual ~game_structure() override;
		bool initialize_sdl(int screen_width, int screen_height);	
		bool initialize();
		bool game_loop();
		void update();
		void world_update() const;
		void player_update();
		void draw(float);
		static long get_tick_now();
		void spare_time(long) const;
		bool unload();	 // releases resources
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> the_event) override;
		std::string get_subscriber_name() override;
	};
}
