#include "SDLGraphicsManager.h"
#include <memory>
#include "SDL_image.h"
#include "GraphicAsset.h"
#include <iostream>
#include <SDL.h>
#include <functional>
#include <SDL_mixer.h>
#include "Window.h"
#include "audio/AudioManager.h"
#include "common/Common.h"
#include "events/EventManager.h"
#include "file/SettingsManager.h"

using namespace std;

namespace gamelib
{
	SDL_Window* SdlGraphicsManager::CreateSdlWindow(const int screenWidth, const int screenHeight, const char* windowTitle, const bool hideWindow)
	{
		Uint32 windowCreationFlags = 0;

		if (hideWindow)
		{
			windowCreationFlags = windowCreationFlags | SDL_WINDOW_HIDDEN;
		}
		else
		{
			windowCreationFlags = windowCreationFlags | SDL_WINDOW_RESIZABLE;
		}

		const auto outWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			screenWidth, screenHeight, windowCreationFlags);

		if (outWindow == nullptr)
		{
			std::cout << "Window could not be created:" << SDL_GetError() << std::endl;
		}
		
		return outWindow;
	}

	SDL_Renderer* SdlGraphicsManager::GetSdlRendererFromWindow(SDL_Window* window)
	{
		const auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		
		if (renderer == nullptr)
		{			
			std::cout << "Renderer could not be created: " << SDL_GetError() << std::endl;
		}
		
		return renderer;
	}
	
	bool SdlGraphicsManager::Initialize(const uint width, const uint height, const char * windowTitle, const bool hideWindow)
	{
		Logger::Get()->LogThis("SDLGraphicsManager::Initialize()", SettingsManager::Bool("global", "verbose"));
				
		// Initialize SDL Image extension/library
		constexpr int imgFlags = IMG_INIT_PNG;
		if(!(IMG_Init(imgFlags) & imgFlags ))
		{
			std::stringstream message;
			message << "SDL_image could not initialize:" << SDL_GetError();

			// TODO: Fix
			THROW(12, message.str(), "SDLGraphicsManager");
		}

		// Create the SdlGraphicsManager's main window
		mainWindow = std::make_shared<Window>(MainWindowName, width, height, windowTitle, hideWindow);
		mainWindow->Initialize();
			
		Logger::Get()->LogThis("SDLGraphicsManager ready.");

		return true;
	}

	void SdlGraphicsManager::ClearAndDraw(const std::function<void(SDL_Renderer* renderer)> &drawObjects) const
	{
		mainWindow->ClearAndDraw(drawObjects);
	}

	uint SdlGraphicsManager::GetScreenWidth() const
	{
		return mainWindow->Width();
	}

	uint SdlGraphicsManager::GetScreenHeight() const
	{
		return mainWindow->Height();
	}

	SdlGraphicsManager::~SdlGraphicsManager()
	{
	    IMG_Quit();
	    SDL_Quit();
		instance = nullptr;
	}

	shared_ptr<Window> SdlGraphicsManager::GetMainWindow()
	{
		return mainWindow;
	}
		
	SdlGraphicsManager* SdlGraphicsManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new SdlGraphicsManager();
		}

		return instance;
	}

	SdlGraphicsManager* SdlGraphicsManager::instance = nullptr;

	SdlGraphicsManager::SdlGraphicsManager() = default;

	vector<shared_ptr<Event>> SdlGraphicsManager::HandleEvent(const std::shared_ptr<Event>& event, const unsigned long deltaMs)
	{
		// We don't subscribe to any events
		return {};	
	}	

	string SdlGraphicsManager::GetSubscriberName()
	{
		return "SDLGraphicsManager";
	}

	void SdlGraphicsManager::Unload()
	{
		IMG_Quit();
	}
	
	std::shared_ptr<GraphicAsset> SdlGraphicsManager::ToGraphicAsset(const std::shared_ptr<Asset>& asset)
	{
		return AsAsset<GraphicAsset>(asset);
	}
}