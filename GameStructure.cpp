#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>
#include <iostream>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "GameStructure.h"
#include <functional>
#include "audio/AudioManager.h"
#include "common/Common.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/DoLogicUpdateEvent.h"
#include "events/EventManager.h"
#include "events/PositionChangeEvent.h"
#include "events/SceneChangedEvent.h"
#include "graphic/SDLGraphicsManager.h"
#include "objects/Player.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"

namespace gamelib
{
	
	/***
	 * Keeps and updated snapshot of the player state
	 */
	void GameStructure::ReadKeyboard()
	{
		// Read from game controller
		get_input_func();	
	}


	vector<shared_ptr<event>> GameStructure::handle_event(std::shared_ptr<event> the_event)
	{
		// GameStructure does not subscribe to any events
		return vector<shared_ptr<event>>();
	}

	string GameStructure::get_subscriber_name()
	{
		return "GameStructure";
	}

	/***
	 * Updates, monitors what the world is doing around the player. This is usually what the player reacts to
	 */
	void GameStructure::world_update() const
	{
		// Send update event to all subscribers
		eventAdmin.dispatch_event_to_subscriber(make_shared<do_logic_update_event>());		
	}

	// This is basically the update functions which is run x FPS to maintain a timed series on constant updates 
	// that simulates constant movement for example or time intervals in a non-time related game (turn based game eg)
	// This is where you would make state changes in the game such as decreasing ammo etc
	void GameStructure::update()
	{	
		// Read input from player
		ReadKeyboard();
		
		// make the game do something now...show game activity that the user will then respond to
		// this generates game play
		world_update();
	}

	// Gets time in milliseconds now
	long GameStructure::get_tick_now()
	{
		return timeGetTime();
	}

	void GameStructure::spare_time(long frame_time) const 	{ eventAdmin.process_all_events(); }

	void GameStructure::draw(float percent_within_tick) { graphics_admin.DrawCurrentScene(scene_admin); }

	bool GameStructure::initialize_sdl(const int screen_width, const int screen_height)
	{
		return log_if_false(graphics_admin.initialize(screen_width, screen_height), "Failed to initialize SDL graphics manager", settings, gameLogger);
	}


	bool GameStructure::unload()
	{
		try 
		{
			resource_admin.unload();		
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


	GameStructure::GameStructure(EventManager& eventAdmin, ResourceManager& resource_admin,
		               SettingsManager& config, game_world_data& world,
		               SceneManager& scene_admin, SDLGraphicsManager& graphics_admin,
		               AudioManager& audio_admin, std::function<void()> get_input_func,
					   Logger& Logger)
			: eventAdmin(eventAdmin), resource_admin(resource_admin),
			settings(config),
			world(world),
			scene_admin(scene_admin),
			graphics_admin(graphics_admin), 
			audio_admin(audio_admin), 
			get_input_func(std::move(get_input_func)),
			gameLogger(Logger) { }

	GameStructure::~GameStructure()
	{
		unload();
	}

	

	/**
	 Initializes resource manager and SDL
	 */
	bool GameStructure::initialize()
	{
		// Initialize the settings manager
		const auto settings_admin_initialized_ok = settings.load("game/settings.xml");
		
		const auto be_verbose = settings.get_bool("global","verbose");
		const auto screen_width = settings.get_int("global", "screen_width");
		const auto screen_height = settings.get_int("global", "screen_height");
		
		return run_and_log("GameStructure::initialize()", be_verbose, [&]()
		{			
			// Initialize resource manager
			const auto resource_admin_initialized_ok = log_if_false(resource_admin.initialize(eventAdmin), "Could not initialize resource manager", settings, gameLogger);

			// Initialize event manager
			const auto eventAdmin_initialized_ok = log_if_false(eventAdmin.initialize(), "Could not initialize event manager", settings, gameLogger);

			// Initialize SceneManager
			const auto scene_admin_initialized_ok = log_if_false(scene_admin.initialize(), "Could not initialsettings->ize scene manager", settings, gameLogger);

			// Initialize SDL
			const auto sdl_initialize_ok = log_if_false(initialize_sdl(screen_width, screen_height), "Could not initialize SDL, aborting.", settings, gameLogger);

			// Final check
			if(failed(sdl_initialize_ok) || 
			   failed(eventAdmin_initialized_ok) ||
			   failed(resource_admin_initialized_ok) || 
			   failed(scene_admin_initialized_ok) ||
			   failed(settings_admin_initialized_ok)) 
				return false;		

			// Start level/scence 1
			scene_admin.start_scene(1);
			
			return true;
		}, settings, true, true);
	}

	/**
	 * Update & Draw until the game ends
	 */
	bool GameStructure::game_loop()
	{
		auto tick_count_at_last_call = get_tick_now();
		const auto max_loops = settings.get_int("global", "max_loops");
		const auto tick_time_ms = settings.get_int("global", "tick_time_ms");
		
		while (!world.is_game_done) // main game loop
		{
			const auto new_time =  get_tick_now();
			auto frame_ticks = 0;  // Number of ticks in the update call	
			auto num_loops = 0;  // Number of loops ??
			auto ticks_since = new_time - tick_count_at_last_call;
			
			// New frame, happens consistently every 50 milliseconds. Ie 20 times a second. 20 times a second = 50 milliseconds. 1 second is 20*50 = 1000 milliseconds
			
			while (ticks_since > tick_time_ms && num_loops < max_loops)
			{
				// update logic
				update();

				tick_count_at_last_call += tick_time_ms; // tickCountAtLastCall is now been +TickTime more since the last time. update it
				frame_ticks += tick_time_ms; num_loops++;
				ticks_since = new_time - tick_count_at_last_call;
			}

			spare_time(frame_ticks); // handle player input, general housekeeping (Event Manager processing)

			if (world.is_network_game || ticks_since <= tick_time_ms)
			{
				if (world.can_render)
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

