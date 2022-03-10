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
	void GameStructure::ReadKeyboard() const
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
		// Read input from player
		ReadKeyboard();

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
		// A 'tick' repreents one update call(). 
		// We want a fixed number of ticks within a real unit of time. A real unit of time does not change (1 second is always one second in the real world)
		// irrespective of what the hardware you are running on. So if we can ensure we have the same number of updates in one second, the update rate
		// will always be te same, ie we have a fixed update rate. 

		// t0 repreesnts the time when the last update() finished.
		// t1 when re-evaluated represents the time elapsed since t0 (last update)

		// t1-t0 is the elapsed time since the last update and is called the frame time, and includes the time taken to draw.
		// The frame time is the time it it takes to since the last update, and which includes subsequent drawing
		// that a new update might be ready to be called again or if not, more drawing can be done.

		// Initialize/prepare process by saying last update occured right now.
		auto t0 = GetTimeNowMs();

		const auto maxLoops = _settingsManager.get_int("global", "max_loops");

		// Get required time(ms) in per update() call. 
		// This is a pre-calculated ms value that will represents a desired fixed number times that update must be called in 1 second,
		// as multiples of this value will equal 1 second.
		const auto TICK_TIME = _settingsManager.get_int("global", "tick_time_ms"); 
		
		while (!_gameWorld.IsGameDone) // main game loop (exact speed of loops is hardware dependaant)
		{
			const auto t1 =  GetTimeNowMs(); // t1 is the time now, and at t0, the last update was called.
			auto elapsedTimeSinceLastUpdateFinished = t1 - t0;			
			
			auto frameTimePerLoop = 0;  // Number of update intervals we could get in 1 game loop (hardware dependant).
			auto num_loops = 0;  // Number of loops 
			
			// Update() needs to be called every x times a second (ie thats pre-defined and therefor fixed)
			// To achieve this means every 1000/20 milliseconds (50ms) update() must be called. 
			// Note: 1 second is 1000 milliseconds, thus 1000/20 - 50ms

			// Update() needs to happen x times a second or every 50 ms 
			// We skip update() if next 50ms interval not yet reached ie elapse time since last update < 50ms (TICK_TIME)
			// However if we are within the next 50ms interval execute update() 
			
			while (elapsedTimeSinceLastUpdateFinished > TICK_TIME && num_loops < maxLoops)
			{
				// +TICK_TIME has just occured, since last update so do another update
				// update logic
				Update();

				// t0 repreesnts the time when the last update() finished.
				// Update it (t0) to +TICK_TIME since that last time it happened.
				t0 += TICK_TIME;

				// Total time so far is what it was plus TICK_TIME (as being > TICK_TIME is what got us here)
				frameTimePerLoop += TICK_TIME; // This should add up to 1000 ms after 1000/TICK_TIME updates.
				num_loops++;
			}

			HandleSpareTime(frameTimePerLoop); // handle player input, general housekeeping (Event Manager processing)

			if (_gameWorld.IsNetworkGame && elapsedTimeSinceLastUpdateFinished > TICK_TIME)
			{
				t0 = t1 - TICK_TIME;
			}

			
			if (_gameWorld.CanDraw)
			{
				const auto percentWithinTick = min(1.0f ,float(elapsedTimeSinceLastUpdateFinished/TICK_TIME)); // NOLINT(bugprone-integer-division)				
				Draw(percentWithinTick);
			}
			cout << frameTimePerLoop/TICK_TIME << " updates/ticks per game loop" << endl;
		}
		std::cout << "Game done" << std::endl;
		return true;
	}
	
}

