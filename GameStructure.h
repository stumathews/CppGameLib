#pragma once
#include "common/Common.h"
#include "events/EventSubscriber.h"
#include "objects/game_world_data.h"
#include "objects/Player.h"
#include "scene/SceneManager.h"
#include <common/Logger.h>

namespace gamelib
{
	/***
	 * logical structure of the game such as the game initialization, game loop etc.
	 */
	class GameStructure final : public EventSubscriber
	{
		EventManager& eventAdmin;
		ResourceManager& resource_admin;
		SettingsManager& settings;		
		SceneManager& scene_admin;
		sdl_graphics_manager& graphics_admin;
		game_world_data& world;
		AudioManager& audio_admin;
		std::function<void()> get_input_func;
		logger& gameLogger;
	public:
		GameStructure(EventManager& event_admin, ResourceManager& resource_admin,
		               SettingsManager& config, game_world_data& world,
		               SceneManager& scene_admin, sdl_graphics_manager& graphics_admin,
		               AudioManager& audio_admin, std::function<void()> get_input_func,
					   logger& gameLogger);
		~GameStructure();
		bool initialize_sdl(int screen_width, int screen_height);	
		bool initialize();
		bool game_loop();
		void update();
		void world_update() const;
		void ReadKeyboard();
		void draw(float);
		static long get_tick_now();
		void spare_time(long) const;
		bool unload();	 // releases resources
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> the_event) override;
		std::string get_subscriber_name() override;
	};
}
