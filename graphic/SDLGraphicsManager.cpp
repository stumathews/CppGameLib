#include "SDLGraphicsManager.h"
#include <memory>
#include "SDL_image.h"
#include "GraphicAsset.h"
#include <iostream>
#include <SDL.h>
#include <functional>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "common/Common.h"
#include "events/EventManager.h"
#include "common/aliases.h"
#include "util/SettingsManager.h"


using namespace std;
namespace gamelib
{

	/// <summary>
	/// Get Instance
	/// </summary>
	/// <returns></returns>
	SDLGraphicsManager* SDLGraphicsManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new SDLGraphicsManager();
		}
		return Instance;
	}

	SDLGraphicsManager* SDLGraphicsManager::Instance = nullptr;

	SDLGraphicsManager::SDLGraphicsManager() = default;

	/// <summary>
	/// Handle any events we've subscribed to
	/// </summary>
	vector<shared_ptr<Event>> SDLGraphicsManager::HandleEvent(const std::shared_ptr<Event> event, unsigned long deltaMs)
	{ 
		return {};	
	}	

	/// <summary>
	/// Provide name to the event system
	/// </summary>
	string SDLGraphicsManager::GetSubscriberName()
	{
		return "SDLGraphicsManager";
	}

	/// <summary>
	/// Converts an Asset to a GraphicAsset
	/// </summary>
	/// <param name="asset"></param>
	/// <returns></returns>
	std::shared_ptr<GraphicAsset> SDLGraphicsManager::ToGraphicAsset(const std::shared_ptr<Asset>& asset)
	{
		return AsAsset<GraphicAsset>(asset);
	}

	/// <summary>
	/// Creates a new SDL Window
	/// </summary>
	SDL_Window* CreateSDLWindow(const int screenWidth, const int screenHeight, const char* title)
	{
		const auto out_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight,SDL_WINDOW_SHOWN);	

		if (out_window == nullptr)
		{
			std::cout << "Window could not be created:" << const_cast<char*>(SDL_GetError()) << std::endl;
		}
		
		return out_window;
	}

	/// <summary>
	/// Get SDL Renderer from Window
	/// </summary>
	/// <param name="window"></param>
	/// <returns></returns>
	SDL_Renderer* GetSDLRendererFromWindow(SDL_Window* window)
	{
		const auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		
		if (renderer == nullptr)
		{			
			std::cout << "Renderer could not be created: " << const_cast<char*>(SDL_GetError()) << std::endl;
		}
		
		return renderer;
	}

	/// <summary>
	/// Initialize the SDL Graphics Manager
	/// </summary>
	bool SDLGraphicsManager::Initialize(const uint width, const uint height, const char * windowTitle)
	{
		Logger::Get()->LogThis("SDLGraphicsManager::Initialize()", SettingsManager::Get()->GetBool("global", "verbose"));
		
		// Initialize SDL Video and Audio subsystems
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0)
		{
			Logger::Get()->LogThis(string("SDL could not initialize:") + const_cast<char*>(SDL_GetError()));
			return false;
		}

		// Initialize SDL Image extension/library
		constexpr int imgFlags = IMG_INIT_PNG;
		if(!(IMG_Init(imgFlags) & imgFlags ))
		{
			THROW(12, string("SDL_image could not initialize:") + const_cast<char*>(SDL_GetError()), "SDLGraphicsManager")
		}

		// Create the Window
		Window = CreateSDLWindow(static_cast<int>(width), static_cast<int>(height), windowTitle);	
		WindowSurface = SDL_GetWindowSurface(Window);
		WindowRenderer = GetSDLRendererFromWindow(Window);
		screenHeight = height;
		screenWidth = width;

		// Set drawing colour to Grey background
		SDL_SetRenderDrawColor(WindowRenderer, 192,192,192, 0);

		// Setup the Audio 
		if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 )
	    {
		    const string message("SDL_mixer could not initialize! SDL_mixer Error: ");
			LogMessage(message + Mix_GetError());
	        return false;
	    }

		// Setup the SDL Font extension/library
		if(TTF_Init() == -1)
		{
			const string message("Could not initialize TTF");
			LogMessage(message);
			return false;			
		}

		Logger::Get()->LogThis("SDLGraphicsManager ready.");
		return true;
	}

	/// <summary>
	/// Clear the output and draw objects on it
	/// </summary>
	void SDLGraphicsManager::ClearAndDraw(const std::function<void(SDL_Renderer* renderer)> &drawObjectsFn) const
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

	void SDLGraphicsManager::ShowOnly() const
	{
		SDL_RenderPresent(WindowRenderer);
	}

	/// <summary>
	/// Uninitialize the SDL Graphics manager
	/// </summary>
	SDLGraphicsManager::~SDLGraphicsManager()
	{
		SDL_DestroyRenderer(WindowRenderer);
		SDL_DestroyWindow(Window);
	    IMG_Quit();
	    SDL_Quit();
		Instance = nullptr;
	}
}