
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
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/DoLogicUpdateEvent.h"
#include "events/event_manager.h"
#include "events/PositionChangeEvent.h"
#include "events/SceneChangedEvent.h"
#include "graphic/sdl_graphics_manager.h"
#include "objects/Player.h"
#include "resource/resource_manager.h"
#include "scene/LevelGenerator.h"
#include "scene/scene_manager.h"

namespace gamelib
{
	void game_structure::get_input()
	{
		SDL_Event sdl_event;
		
		while(SDL_PollEvent(&sdl_event) != 0)
		{
			if(sdl_event.type != SDL_QUIT)
			{
				if (sdl_event.type == SDL_KEYDOWN)
				{
					switch (sdl_event.key.keysym.sym)
					{
					case SDLK_w:
					case SDLK_UP:
						run_and_log("Player pressed up!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<position_change_event>(Direction::Up), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_s:
					case SDLK_DOWN:
						run_and_log("Player pressed down!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<position_change_event>(Direction::Down), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_a:
					case SDLK_LEFT:
						run_and_log("Player pressed left!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<position_change_event>(Direction::Left), this);
							return true;
						}, true, true, settings_admin);
						break;

					case SDLK_d:
					case SDLK_RIGHT:
						run_and_log("Player pressed right!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<position_change_event>(Direction::Right), this);
							return true;
						}, true, true, settings_admin);
						break;

					case SDLK_q:
					case SDLK_ESCAPE:
						run_and_log("Player pressed quit!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							game_world->is_game_done = 1;
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_j:
						run_and_log("Change to level 1", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<scene_changed_event>(1), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_k:
						run_and_log("Change to level 2", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<scene_changed_event>(2), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_l:
						run_and_log("Change to level 3", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<scene_changed_event>(3), this);
							return true;
						}, true, true, settings_admin);
						break;

					case SDLK_x:
						run_and_log("Change to level 4", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<scene_changed_event>(4), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_1:
						Mix_PlayChannel(-1, audio_manager::to_resource(resource_admin->get("high.wav"))->as_fx(), 0);
						break;
					case SDLK_2:
						Mix_PlayChannel(-1, audio_manager::to_resource(resource_admin->get("medium.wav"))->as_fx(), 0);
						break;
					case SDLK_3:
						Mix_PlayChannel(-1,  audio_manager::to_resource(resource_admin->get("low.wav"))->as_fx(), 0);
						break;
					case SDLK_4:
						Mix_PlayChannel(-1, audio_manager::to_resource(resource_admin->get("scratch.wav"))->as_fx(), 0);
						break;
					case SDLK_9:
						if (Mix_PlayingMusic() == 0)
						{
							Mix_PlayMusic(audio_manager::to_resource(resource_admin->get("MainTheme.wav"))->as_music(), -1);
						}
						else
						{
							if (Mix_PausedMusic() == 1)
								Mix_ResumeMusic();
							else
								Mix_PauseMusic();
						}
						break;
					case SDLK_0:
						Mix_HaltMusic();
						break;
					case SDLK_r:
						settings_admin->reload();
						log_message("Settings reloaded", settings_admin->get_bool("global", "verbose"), false);
						break;
					default:
						std::cout << "Unknown control key" << std::endl;
						log_message("Unknown control key", settings_admin->get_bool("global", "verbose"));
						break;
					}
				}
			}
			else
			{
				game_world->is_game_done = true;
			}
		}
	}

	/***
	 * Keeps and updated snapshot of the player state
	 */
	void game_structure::player_update()
	{
		// Read from game controller
		get_input();	
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
		std::shared_ptr<game_world_data> game_world,
		std::shared_ptr<scene_manager> scene_admin,
		std::shared_ptr<sdl_graphics_manager> graphics_admin)
	: event_admin(event_admin),
	resource_admin(resource_admin),
	settings_admin(config),
	game_world(game_world),
	scene_admin(scene_admin),
	graphics_admin(graphics_admin)
	{
		
		init_game_world_data();
	}

	void game_structure::init_game_world_data() const
	{
		game_world->is_game_done = false;
		game_world->is_network_game = false;
		game_world->can_render = true;
	}

	shared_ptr<player> game_structure::create_player() const
	{
		return make_shared<player>(player(settings_admin->get_int("player","player_init_pos_x"), settings_admin->get_int("player", "player_init_pos_y"), settings_admin->get_int("global", "square_width") / 2, resource_admin, settings_admin));
	}

	void game_structure::setup_player() const
	{
		const auto the_player = create_player();	
		the_player->subscribe_to_event(event_type::PositionChangeEventType, event_admin);	
		the_player->raise_event(std::make_shared<add_game_object_to_current_scene_event>(the_player, 100), event_admin);
	}

	/**
	 Initializes resource manager and SDL
	 */
	bool game_structure::initialize()
	{
		// Initialize the settings mnager
		const auto settings_admin_initialized_ok = settings_admin->load("game/settings.xml");
		return run_and_log("game_structure::initialize()", settings_admin->get_bool("global","verbose"), [&]()
		{			
			// Initialize resource manager
			const auto resource_admin_initialized_ok = log_if_false(resource_admin->initialize(event_admin), "Could not initialize resource manager");

			// Initialize event manager
			const auto event_admin_initialized_ok = log_if_false(event_admin->initialize(), "Could not initialize event manager");

			// Initialize scene_manager
			const auto scene_admin_initialized_ok = log_if_false(scene_admin->initialize(), "Could not initialize scene manager");

			// Initialize SDL
			const auto sdl_initialize_ok = log_if_false(initialize_sdl(settings_admin->get_int("global", "screen_width"), settings_admin->get_int("global", "screen_height")), "Could not initialize SDL, aborting.");
			
			if(failed(sdl_initialize_ok) || 
			   failed(event_admin_initialized_ok) ||
			   failed(resource_admin_initialized_ok) || 
			   failed(scene_admin_initialized_ok) ||
				failed(settings_admin_initialized_ok)) 
				return false;		

			// Start the first scene
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
				
		while (!game_world->is_game_done) // main game loop
		{
			const auto new_time =  get_tick_now();
			auto frame_ticks = 0;  // Number of ticks in the update call	
			auto num_loops = 0;  // Number of loops ??
			auto ticks_since = new_time - tick_count_at_last_call;
			auto tick_time_ms = settings_admin->get_int("global", "tick_time_ms");
			/* New frame, happens consistently every 50 milliseconds. Ie 20 times a second.
			  20 times a second = 50 milliseconds
			  1 second is 20*50 = 1000 milliseconds
			*/
			while (ticks_since > settings_admin->get_int("global", "tick_time_ms") && num_loops < max_loops)
			{
				update();		
				tick_count_at_last_call += tick_time_ms; // tickCountAtLastCall is now been +TickTime more since the last time. update it
				frame_ticks += tick_time_ms; num_loops++;
				ticks_since = new_time - tick_count_at_last_call;
			}

			spare_time(frame_ticks); // handle player input, general housekeeping (Event Manager processing)

			if (game_world->is_network_game || ticks_since <= tick_time_ms)
			{
				if (game_world->can_render)
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


	bool game_structure::load_content() const
	{
		resource_admin->read_resources();
			
		// Generate the level's rooms
		for (const auto& room: level_generator::generate_level(resource_admin, settings_admin))
		{	
			// room's will want to know when the player moved for collision detection etc
			room->subscribe_to_event(event_type::PlayerMovedEventType, event_admin);
			
			// Add each room to the scene
			room->raise_event(std::make_shared<add_game_object_to_current_scene_event>(std::dynamic_pointer_cast<square>(room)), event_admin);	
		}

		// Create the player
		setup_player();

		return true;	
	}

}

