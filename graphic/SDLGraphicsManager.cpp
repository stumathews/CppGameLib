#include "SDLGraphicsManager.h"
#include <memory>
#include "SDL_image.h"
#include "GraphicAsset.h"
#include <iostream>
#include <SDL.h>
#include <functional>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "audio/AudioManager.h"
#include "common/Common.h"
#include "events/EventManager.h"
#include "common/aliases.h"
#include "font/FontManager.h"
#include "util/SettingsManager.h"

using namespace std;

namespace gamelib
{
	SDL_Window* CreateSdlWindow(const int screenWidth, const int screenHeight, const char* windowTitle)
	{
		const auto outWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight,SDL_WINDOW_SHOWN);	

		if (outWindow == nullptr)
		{
			std::cout << "Window could not be created:" << SDL_GetError() << std::endl;
		}
		
		return outWindow;
	}

	SDL_Renderer* GetSdlRendererFromWindow(SDL_Window* window)
	{
		const auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		
		if (renderer == nullptr)
		{			
			std::cout << "Renderer could not be created: " << SDL_GetError() << std::endl;
		}
		
		return renderer;
	}


	bool SdlGraphicsManager::Initialize(const uint width, const uint height, const char * windowTitle)
	{
		Logger::Get()->LogThis("SDLGraphicsManager::Initialize()", SettingsManager::Bool("global", "verbose"));
		
		
		// Initialize SDL Image extension/library
		constexpr int imgFlags = IMG_INIT_PNG;
		if(!(IMG_Init(imgFlags) & imgFlags ))
		{
			std::stringstream message;
			message << "SDL_image could not initialize:" << SDL_GetError();

			// TODO: Fix
			THROW(12, message.str(), "SDLGraphicsManager")
		}

		// Create the Window
		window = CreateSdlWindow(static_cast<int>(width), static_cast<int>(height), windowTitle);	
		windowSurface = SDL_GetWindowSurface(window);
		WindowRenderer = GetSdlRendererFromWindow(window);
		screenHeight = height;
		screenWidth = width;

		// Set drawing colour to Grey background
		SDL_SetRenderDrawColor(WindowRenderer, 192,192,192, 0);
			

		Logger::Get()->LogThis("SDLGraphicsManager ready.");

		return true;
	}


	void SdlGraphicsManager::ClearAndDraw(const std::function<void(SDL_Renderer* renderer)> &drawObjectsFn) const
	{
		// Backup current render color
		SDL_Color oldColour = {0, 0, 0, 0};
		SDL_GetRenderDrawColor(WindowRenderer, &oldColour.r, &oldColour.g, &oldColour.b, &oldColour.a);
		
		// Clear
		SDL_RenderClear(WindowRenderer);

		// Draw everything
		drawObjectsFn(WindowRenderer);
		
		// Set draw colour
		SDL_SetRenderDrawColor(WindowRenderer, oldColour.r, oldColour.g, oldColour.b, oldColour.a); // nb: restore whatever the render routine set as the draw color to
		
		// Show what we've drawn
		SDL_RenderPresent(WindowRenderer);
	}

	void SdlGraphicsManager::ShowOnly() const
	{
		SDL_RenderPresent(WindowRenderer);
	}
	
	SdlGraphicsManager::~SdlGraphicsManager()
	{
		SDL_DestroyRenderer(WindowRenderer);
		SDL_DestroyWindow(window);
	    IMG_Quit();
	    SDL_Quit();
		instance = nullptr;
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

	vector<shared_ptr<Event>> SdlGraphicsManager::HandleEvent(const std::shared_ptr<Event> event, unsigned long deltaMs)
	{ 
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