
#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>
#include <iostream>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "game_structure.h"
#include <functional>
#include "audio/AudioManager.h"
#include "common/Common.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/DoLogicUpdateEvent.h"
#include "events/event_manager.h"
#include "events/PositionChangeEvent.h"
#include "events/SceneChangedEvent.h"
#include "graphic/sdl_graphics_manager.h"
#include "objects/Player.h"
#include "resource/resource_manager.h"
#include "scene/scene_manager.h"

namespace gamelib
{
	
	/***
	 * Keeps and updated snapshot of the player state
	 */
	void game_structure::player_update()
	{
		// Read from game controller
		get_input_func();	
	}


	vector<shared_ptr<event>> game_structure::handle_event(std::shared_ptr<event> the_event)
	{
		// game_structure does not subscribe to any events
		return vector<shared_ptr<event>>();
	}

	string game_structure::get_subscriber_name()
	{
		return "game_structure";
	}

	/***
	 * Updates, monitors what the world is doing around the player. This is usually what the player reacts to
	 */
	void game_structure::world_update() const
	{
		const auto update_event = make_shared<do_logic_update_event>();
		// Ask the event manager to notify event subscribers to update their logic now
		//event_admin->raise_event(update_event, this);
		event_admin->dispatch_event_to_subscriber(update_event);
		
	}

	// This is basically the update functions which is run x FPS to maintain a timed series on constant updates 
	// that simulates constant movement for example or time intervals in a non-time related game (turn based game eg)
	// This is where you would make state changes in the game such as decreasing ammo etc
	void game_structure::update()
	{	
		// This game logic keeps the world simulator running:
		player_update();
		
		// make the game do something now...show game activity that the user will then respond to
		// this generates game play
		world_update();
	}

	// Gets time in milliseconds now
	long game_structure::get_tick_now()
	{
		return timeGetTime();
	}

	void game_structure::spare_time(long frame_time) const
	{
		event_admin->process_all_events();
	}

	void game_structure::draw(float percent_within_tick)
	{
		graphics_admin->draw_current_scene(scene_admin);
	}

	bool game_structure::initialize_sdl(const int screen_width, const int screen_height)
	{
		return log_if_false(graphics_admin->initialize(screen_width, screen_height),"Failed to initialize SDL graphics manager");
	}


	bool game_structure::unload()
	{
		try 
		{
			resource_admin->unload();		
			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
				
			return true;
		}
		catch(...)
		{
			return false;
		}
	}


	game_structure::game_structure(std::shared_ptr<event_manager> event_admin,
		std::shared_ptr<resource_manager> resource_admin,
		std::shared_ptr<settings_manager> config,
		std::shared_ptr<game_world_data> world,
		std::shared_ptr<scene_manager> scene_admin,
		std::shared_ptr<sdl_graphics_manager> graphics_admin, std::shared_ptr<audio_manager> audio_admin, std::function<void()> get_input_func)
	: event_admin(event_admin),
	resource_admin(resource_admin),
	settings_admin(config),
	world(world),
	scene_admin(scene_admin),
	graphics_admin(graphics_admin), audio_admin(std::move(audio_admin)), get_input_func(std::move(get_input_func))
{
	}

	game_structure::~game_structure()
	{
		unload();
	}

	

	/**
	 Initializes resource manager and SDL
	 */
	bool game_structure::initialize()
	{
		// Initialize the settings manager
		const auto settings_admin_initialized_ok = settings_admin->load("game/settings.xml");
		
		const auto be_verbose = settings_admin->get_bool("global","verbose");
		const auto screen_width = settings_admin->get_int("global", "screen_width");
		const auto screen_height = settings_admin->get_int("global", "screen_height");
		
		return run_and_log("game_structure::initialize()", be_verbose, [&]()
		{			
			// Initialize resource manager
			const auto resource_admin_initialized_ok = log_if_false(resource_admin->initialize(event_admin), "Could not initialize resource manager", settings_admin);

			// Initialize event manager
			const auto event_admin_initialized_ok = log_if_false(event_admin->initialize(), "Could not initialize event manager", settings_admin);

			// Initialize scene_manager
			const auto scene_admin_initialized_ok = log_if_false(scene_admin->initialize(), "Could not initialize scene manager", settings_admin);

			// Initialize SDL
			const auto sdl_initialize_ok = log_if_false(initialize_sdl(screen_width, screen_height), "Could not initialize SDL, aborting.", settings_admin);

			// Final check
			if(failed(sdl_initialize_ok) || 
			   failed(event_admin_initialized_ok) ||
			   failed(resource_admin_initialized_ok) || 
			   failed(scene_admin_initialized_ok) ||
			   failed(settings_admin_initialized_ok)) 
				return false;		

			// Start level/scence 1
			scene_admin->start_scene(1);
			
			return true;
		}, true, true, settings_admin);
	}

	/**
	 * Update & Draw until the game ends
	 */
	bool game_structure::game_loop()
	{
		auto tick_count_at_last_call = get_tick_now();
		const auto max_loops = settings_admin->get_int("global", "max_loops");
		const auto tick_time_ms = settings_admin->get_int("global", "tick_time_ms");
		
		while (!world->is_game_done) // main game loop
		{
			const auto new_time =  get_tick_now();
			auto frame_ticks = 0;  // Number of ticks in the update call	
			auto num_loops = 0;  // Number of loops ??
			auto ticks_since = new_time - tick_count_at_last_call;
			
			// New frame, happens consistently every 50 milliseconds. Ie 20 times a second. 20 times a second = 50 milliseconds. 1 second is 20*50 = 1000 milliseconds
			
			while (ticks_since > settings_admin->get_int("global", "tick_time_ms") && num_loops < max_loops)
			{
				update();		
				tick_count_at_last_call += tick_time_ms; // tickCountAtLastCall is now been +TickTime more since the last time. update it
				frame_ticks += tick_time_ms; num_loops++;
				ticks_since = new_time - tick_count_at_last_call;
			}

			spare_time(frame_ticks); // handle player input, general housekeeping (Event Manager processing)

			if (world->is_network_game || ticks_since <= tick_time_ms)
			{
				if (world->can_render)
				{
					const auto percent_outside_frame = static_cast<float>(ticks_since / tick_time_ms) * 100; // NOLINT(bugprone-integer-division)				
					draw(percent_outside_frame);
				}
			}
			else
			{
				tick_count_at_last_call = new_time - tick_time_ms;
			}
		}
		std::cout << "Game done" << std::endl;
		return true;
	}
	
}

