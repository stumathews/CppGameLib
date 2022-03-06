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

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Create the game structure
	/// </summary>
	/// <param name="eventAdmin">Event manager</param>
	/// <param name="resource_admin">Resource manager</param>
	/// <param name="config">Settings manager</param>
	/// <param name="world"></param>
	/// <param name="scene_admin"></param>
	/// <param name="graphics_admin"></param>
	/// <param name="audio_admin"></param>
	/// <param name="get_input_func"></param>
	/// <param name="Logger"></param>
	GameStructure::GameStructure(EventManager& eventManager, 
						ResourceManager& resourceManager,
		                SettingsManager& settingsManager, GameWorldData& world,
		                SceneManager& sceneManager, SDLGraphicsManager& graphicsManager,
		                AudioManager& audoManager, std::function<void()> getControllerInputFunction,
					    Logger& Logger)
			: _eventManager(eventManager), 
			_resourceManager(resourceManager),
			_settingsManager(settingsManager),
			_gameWorld(world),
			_sceneManager(sceneManager),
			_graphicsManager(graphicsManager), 
			_audioManager(audoManager), 
			_getControllerInputFunction(std::move(getControllerInputFunction)),
			_gameLogger(Logger) { }

	/// <summary>
	/// tear down and unloda game subsystems when the game shuts down
	/// </summary>
	GameStructure::~GameStructure()
	{
		UnloadGameSubsystems();
	}
	
	/// <summary>
	///  Initializes game subsystems resource manager and SDL	
	/// </summary>
	/// <returns>true if subsystems initialised, false otherwise</returns>
	bool GameStructure::InitializeGameSubSystems()
	{
		// Initialize the settings manager
		const auto settings_admin_initialized_ok = _settingsManager.load("game/settings.xml");
		
		// Load key game subsystem settings
		const auto be_verbose = _settingsManager.get_bool("global","verbose");
		const auto screen_width = _settingsManager.get_int("global", "screen_width");
		const auto screen_height = _settingsManager.get_int("global", "screen_height");
		
		// Perform the initialiation
		return LogThis("GameStructure::initialize()", be_verbose, [&]()
		{			
			// Initialize resource manager
			const auto resource_admin_initialized_ok = LogOnFailure(_resourceManager.Initialize(_eventManager), "Could not initialize resource manager", _settingsManager, _gameLogger);

			// Initialize event manager
			const auto eventAdmin_initialized_ok = LogOnFailure(_eventManager.Initialize(), "Could not initialize event manager", _settingsManager, _gameLogger);

			// Initialize SceneManager
			const auto scene_admin_initialized_ok = LogOnFailure(_sceneManager.Initialize(), "Could not initialsettings->ize scene manager", _settingsManager, _gameLogger);

			// Initialize SDL
			const auto sdl_initialize_ok = LogOnFailure(InitializeSDL(screen_width, screen_height), "Could not initialize SDL, aborting.", _settingsManager, _gameLogger);

			// Final check to see if all subsystems are initialised ok
			if(failed(sdl_initialize_ok, _gameLogger) || 
			   failed(eventAdmin_initialized_ok, _gameLogger) ||
			   failed(resource_admin_initialized_ok, _gameLogger) || 
			   failed(scene_admin_initialized_ok, _gameLogger) ||
			   failed(settings_admin_initialized_ok, _gameLogger)) 
				return false;	
						
			return true;
		}, _settingsManager, true, true);
	}

	
	/// <summary>
	/// Keeps and updated snapshot of the player state
	/// </summary>
	void GameStructure::ReadKeyboard()
	{
		// Read from game controller
		_getControllerInputFunction();	
	}

	/// <summary>
	/// Handle top-level game events
	/// </summary>
	/// <param name="the_event">The incoming event</param>
	/// <returns>List of generated events that occured while processing this event</returns>
	vector<shared_ptr<Event>> GameStructure::HandleEvent(std::shared_ptr<Event> the_event)
	{
		// GameStructure does not subscribe to any events
		return vector<shared_ptr<Event>>();
	}

	/// <summary>
	/// Indicate who we are in the event system
	/// </summary>
	/// <returns></returns>
	string GameStructure::GetSubscriberName()
	{
		return "Game";
	}

	
	/// <summary>
	/// Updates, monitors what the world is doing around the player. This is usually what the player reacts to
	/// </summary>
	void GameStructure::UpdateWorld() const
	{
		// Send Update event to all subscribers who support updates
		_eventManager.DispatchEventToSubscriber(make_shared<do_logic_update_event>());		
	}

	/// <summary>
	/// Update logic in game.
	/// Is run x FPS to maintain a timed series on constant updates
	/// </summary>
	void GameStructure::Update()
	{	
		// Read input from player
		ReadKeyboard();
		
		// make the game do something now...show game activity that the user will then respond to
		// this generates game play
		UpdateWorld();
	}

	/// <summary>
	/// Gets time in milliseconds now
	/// </summary>
	/// <returns>Time In Milliseconds</returns>
	long GameStructure::GetTimeNowMs() 
	{
		return timeGetTime();
	}

	/// <summary>
	/// Process all pending events
	/// </summary>
	/// <param name="frame_time"></param>
	void GameStructure::HandleSpareTime(long frame_time) const 	
	{
		// Contacts all event subscribers for all events that are currently waiting to be processed in the event queue
		_eventManager.ProcessAllEvents(); 
	}

	/// <summary>
	/// Draws the game
	/// </summary>
	/// <param name="percent_within_tick"></param>
	void GameStructure::Draw(float percent_within_tick) 
	{
		// Draws the current scene
		_graphicsManager.DrawCurrentScene(_sceneManager); 
	}

	/// <summary>
	/// Initialized SDL
	/// </summary>
	/// <param name="screen_width">Width of the screen</param>
	/// <param name="screen_height">Height of the screen</param>
	/// <returns>true if SDL is successfully initialized, false otherwise</returns>
	bool GameStructure::InitializeSDL(const int screenWidth, const int screenHeight)
	{
		return LogOnFailure(_graphicsManager.initialize(screenWidth, screenHeight), "Failed to initialize SDL graphics manager", _settingsManager, _gameLogger);
	}


	/// <summary>
	/// Unload and uninitialize key game sybsystems incl SDL, Image and font libraries and the resource manager
	/// </summary>
	/// <returns>true if unloading succeeded, false otherwise</returns>
	bool GameStructure::UnloadGameSubsystems()
	{
		try 
		{
			_resourceManager.Unload();		
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

	/// <summary>
	/// Update & Draw until the game ends
	/// </summary>
	/// <returns></returns>
	bool GameStructure::DoGameLoop()
	{
		auto tick_count_at_last_call = GetTimeNowMs();
		const auto max_loops = _settingsManager.get_int("global", "max_loops");
		const auto tick_time_ms = _settingsManager.get_int("global", "tick_time_ms");
		
		while (!_gameWorld.IsGameDone) // main game loop
		{
			const auto new_time =  GetTimeNowMs();
			auto frame_ticks = 0;  // Number of ticks in the update call	
			auto num_loops = 0;  // Number of loops ??
			auto ticks_since = new_time - tick_count_at_last_call;
			
			// New frame, happens consistently every 50 milliseconds. Ie 20 times a second. 20 times a second = 50 milliseconds. 1 second is 20*50 = 1000 milliseconds
			
			while (ticks_since > tick_time_ms && num_loops < max_loops)
			{
				// update logic
				Update();

				tick_count_at_last_call += tick_time_ms; // tickCountAtLastCall is now been +TickTime more since the last time. update it
				frame_ticks += tick_time_ms; num_loops++;
				ticks_since = new_time - tick_count_at_last_call;
			}

			HandleSpareTime(frame_ticks); // handle player input, general housekeeping (Event Manager processing)

			if (_gameWorld.IsNetworkGame || ticks_since <= tick_time_ms)
			{
				if (_gameWorld.CanDraw)
				{
					const auto percent_outside_frame = static_cast<float>(ticks_since / tick_time_ms) * 100; // NOLINT(bugprone-integer-division)				
					Draw(percent_outside_frame);
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

