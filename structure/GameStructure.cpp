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
#include "VariableGameLoop.h"
#include "events/EventFactory.h"
#include "font/FontManager.h"
#include "file/SettingsManager.h"

using namespace std;

namespace gamelib
{

	GameStructure::GameStructure(std::shared_ptr<IGameLoopStrategy> gameLoop): gameLoop(std::move(gameLoop))
	{
	}

	GameStructure::GameStructure(std::function<void(unsigned long deltaMs)> getInputFunction)
		: getControllerInputFunction(std::move(getInputFunction)), gameLoop(nullptr)
	{
		// We use the old variable game loop if we don't specify a specific one
		gameLoop =  make_shared<VariableGameLoop>(
			[&](const unsigned long deltaMs) { Update(deltaMs); },
			[&]() { Draw(0UL);});		
	}
	/// <summary>
	/// Update & Draw until the game ends
	/// </summary>
	/// <returns></returns>
	bool GameStructure::DoGameLoop(const GameWorldData* gameWorldData) const
	{
		gameLoop->Loop(gameWorldData);
		std::cout << "Game done" << std::endl;
		return true;
	}

	bool GameStructure::Initialize(int screenWidth, int screenHeight, const string& windowTitle,
	                                             const string resourceFilePath, const string& gameSettingsFilePath)
	{
		// Read config about the game's settings
		const auto settingsInitialized = SettingsManager::Get()->Load(gameSettingsFilePath);
		const auto beVerbose = SettingsManager::Bool("global", "verbose");
		sampleInput = SettingsManager::Bool("gameStructure", "sampleInput");
		sampleNetwork = SettingsManager::Bool("gameStructure", "sampleNetwork");

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
		
		ReadKeyboard(deltaMs);
		ReadNetwork();

		EventManager::Get()->ProcessAllEvents(deltaMs);
		EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateUpdateAllGameObjectsEvent(), deltaMs);
		EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateUpdateProcessesEvent(), deltaMs);
		std::cout << deltaMs <<  " ";
		
	}

	void GameStructure::Draw(unsigned long percentWithinTick)
	{
		// Time-sensitive, skip queue. Draws the current scene
		EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateGenericEvent(DrawCurrentSceneEventId), 0UL);
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

	void GameStructure::ReadKeyboard(const unsigned long deltaMs) const
	{
		if(!sampleInput) return;

		getControllerInputFunction(deltaMs);
	}
	void GameStructure::ReadNetwork() const
	{
		if(!sampleNetwork) return;
		NetworkManager::Get()->Listen();
	}

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
