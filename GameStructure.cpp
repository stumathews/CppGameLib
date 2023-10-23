#include <SDL_image.h>
#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include "GameStructure.h"
#include <functional>
#include <SDL_ttf.h>
#include "audio/AudioManager.h"
#include "common/Common.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/UpdateAllGameObjectsEvent.h"
#include "events/EventManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"
#include <string>
#include <net/NetworkManager.h>
#include "Logging/ErrorLogManager.h"
#include <events/UpdateProcessesEvent.h>
#include "font/FontManager.h"
#include "file/SettingsManager.h"

using namespace std;

namespace gamelib
{
	GameStructure::GameStructure(std::function<void()> getControllerInputFunction)
		: _getControllerInputFunction(std::move(getControllerInputFunction))
	{
	}

	/// <summary>
	/// Update & Draw until the game ends
	/// </summary>
	/// <returns></returns>
	bool GameStructure::DoGameLoop(const GameWorldData* gameWorldData) const
	{
		const auto maxUpdates = SettingsManager::Int("global", "max_loops");

		// Tick/update every tick_time_ms, ie if tick_time_ms = 10, then will tick/update 10 times in one second
		const auto TICK_TIME = SettingsManager::Int("global", "tick_time_ms");

		// A 'tick' reprents one update call(). 
		// We want a fixed number of ticks within a real unit of time. 
		// A real unit of time does not change (1 second is always one second in the real world)
		// irrespective of what the hardware you are running on. So if we can ensure we have the same number of updates in one second, the update rate
		// will always be te same, the we have a fixed update rate. 

		// t0 repreesnts the time when the last update() finished.
		// t1 when re-evaluated represents the time elapsed since t0 (last update)

		// t1-t0 is the elapsed time since the last update and is called the frame-time, and includes the time taken to draw.
		// The frame-time is the time it it takes to since the last update, and which includes subsequent drawing
		// that a new update might be ready to be called again or if not, more drawing can be done.

		// Initialize/prepare process by saying last update occured right now.
		auto t0 = GetTimeNowMs();

		while (!gameWorldData->IsGameDone) // main game loop (exact speed of loops is hardware dependaant)
		{
			const auto t1 = GetTimeNowMs(); // t1 is the time now, and at t0, the last update was called.
			// t1 has been affected by the time to update and draw.

			auto elapsedTime = 0; // Elapsed time in counted as number of updates per 1 game loop (hardware dependant).
			auto countUpdates = 0; // Number of loops 

			// Update() can be called every tick_time_ms which will result in a constant amount of ticks, as a ms is a ms independant on hardware.			
			// Update() will 'tick' x times a second, depending on how long a tick is set to be, ie. tick_time_ms

			// updating time!

			// allow for multiple successive updates if the previous a) drawing b) sparetime operations (t1) took too long (longer than
			// 1 tick_time_ms and so we couldn't do an update, so make up for it here)
			// This only means it executes the right number of updates within a second, not that they have the same interval between them
			while ((t1 - t0) > TICK_TIME && countUpdates < maxUpdates)
			{
				// +TICK_TIME has just occured, since last update so do another update
				// update logic
				Update(t1 - t0);

				t0 += TICK_TIME;

				elapsedTime += TICK_TIME;
				countUpdates++;
			}

			// at this point the stats we have are: 
			// 1) the elapsed time taken in 1 hardware loop
			// 2) the number of updates in 1 hardware loop, while (still ensuring we alway update every tick_time_ms)

			// Misc tasks

			HandleSpareTime(elapsedTime); // handle player input, general housekeeping (Event Manager processing)

			// drawing time!

			if (gameWorldData->IsNetworkGame && (t1 - t0) > TICK_TIME) { t0 = t1 - TICK_TIME; }

			if (gameWorldData->CanDraw)
			{
				// How much within the new 'tick' are we?
				const auto percentWithinTick = min(1.0f, (t1 - t0) / TICK_TIME);
				// NOLINT(bugprone-integer-division, clang-diagnostic-implicit-int-float-conversion)				
				Draw(static_cast<unsigned int>(percentWithinTick));
			}
		}
		std::cout << "Game done" << std::endl;
		return true;
	}

	bool GameStructure::Initialize(int screenWidth, int screenHeight, const string& windowTitle,
	                                             const string resourceFilePath, const string& gameSettingsFilePath)
	{
		// Read config about the game's settings
		const auto settingsInitialized = SettingsManager::Get()->Load(gameSettingsFilePath);
		const auto beVerbose = SettingsManager::Bool("global", "verbose");

		if (screenWidth == 0) { screenWidth = SettingsManager::Int("global", "screen_width"); }
		if (screenHeight == 0) { screenHeight = SettingsManager::Int("global", "screen_height"); }

		// Perform the initialization
		return LogThis("GameStructure::initialize()", beVerbose, [&]()
		{
			if (SettingsManager::Bool("global", "isNetworkGame"))
			{
				LogOnFailure(NetworkManager::Get()->Initialize(), "Could not initialize network manager");
			}

			const auto title = SettingsManager::Bool("global", "isNetworkGame")
				                   ? NetworkManager::Get()->IsGameServer()
					                     ? windowTitle + " (Multi-player - Server)"
					                     : windowTitle + " (Multi-player - Client)"
				                   : windowTitle + " - Single Player Mode";

			// Final check to see if all subsystems are initialized ok
			if (IsFailedOrFalse(LogOnFailure(InitializeSdl(screenWidth, screenHeight, title),
			                                 "Could not initialize SDL, aborting.")) ||
				IsFailedOrFalse(LogOnFailure(EventManager::Get()->Initialize(),
				                             "Could not initialize event manager")) ||
				IsFailedOrFalse(LogOnFailure(ResourceManager::Get()->Initialize(resourceFilePath),
				                             "Could not initialize resource manager")) ||
				IsFailedOrFalse(LogOnFailure(SceneManager::Get()->Initialize(),
				                             "Could not initialize scene manager")) ||
				IsFailedOrFalse(settingsInitialized)) { return false; }

			return true;
		}, true, true);
	}

	void GameStructure::Update(const unsigned long deltaMs) const
	{
		ReadKeyboard();
		ReadNetwork();

		EventManager::Get()->ProcessAllEvents(deltaMs);
		EventManager::Get()->DispatchEventToSubscriber(make_shared<UpdateAllGameObjectsEvent>(), deltaMs);
		EventManager::Get()->DispatchEventToSubscriber(make_shared<UpdateProcessesEvent>(), deltaMs);
	}

	void GameStructure::Draw(unsigned long percentWithinTick) const
	{
		// Time-sensitive, skip queue. Draws the current scene
		EventManager::Get()->DispatchEventToSubscriber(std::make_shared<Event>(DrawCurrentSceneEventId), 0UL);
	}


	bool GameStructure::InitializeSdl(const int screenWidth, const int screenHeight, const string& windowTitle)
	{
		// Initialize SDL Video and Audio subsystems
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		{
			std::stringstream message;
			message << "SDL could not initialize:" << SDL_GetError();

			Logger::Get()->LogThis(message.str());

			return false;
		}

		// Initialise Graphics, Audio and Font subsystems
		const auto graphicsSystemInitialized = SdlGraphicsManager::Get()->Initialize(
			screenWidth, screenHeight, windowTitle.c_str());
		const auto audioSystemInitialized = AudioManager::Initialize();
		const auto fontSystemInitialized = FontManager::Initialize();

		const auto allSystemsInitialized = graphicsSystemInitialized || audioSystemInitialized || fontSystemInitialized;

		return LogOnFailure(allSystemsInitialized, "Failed to initialize SDL graphics manager");
	}

	bool GameStructure::Unload()
	{
		try
		{
			// Unload all the content currently stored in memory
			ResourceManager::Get()->Unload();

			// Finish/Finalize library usage

			FontManager::Unload();
			SdlGraphicsManager::Unload();

			SDL_Quit();

			// All was well
			return true;
		}
		catch (exception& e)
		{
			// Something went wrong
			ErrorLogManager::GetErrorLogManager()->LogMessage(e.what());
			return false;
		}
	}

	void GameStructure::ReadKeyboard() const { _getControllerInputFunction(); }
	void GameStructure::ReadNetwork() { NetworkManager::Get()->Listen(); }

	void GameStructure::HandleSpareTime(long elapsedTime)
	{
	}

	vector<shared_ptr<Event>> GameStructure::HandleEvent(std::shared_ptr<Event> the_event, unsigned long deltaMs)
	{
		return {};
	}

	string GameStructure::GetSubscriberName()
	{
		return "Game";
	}

	long GameStructure::GetTimeNowMs()
	{
		return static_cast<long>(timeGetTime());
	}

	GameStructure::~GameStructure()
	{
		Unload();
	}
}
