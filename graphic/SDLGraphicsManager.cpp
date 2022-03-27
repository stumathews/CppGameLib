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
#include "scene/SceneManager.h"

using namespace std;
namespace gamelib
{

	SDLGraphicsManager* SDLGraphicsManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new SDLGraphicsManager();
		}
		return Instance;
	}

	SDLGraphicsManager* SDLGraphicsManager::Instance = nullptr;

	SDLGraphicsManager::SDLGraphicsManager() : EventSubscriber() { }

	/// <summary>
	/// Handle any events we've subscribed to
	/// </summary>
	vector<shared_ptr<Event>> SDLGraphicsManager::HandleEvent(const std::shared_ptr<Event> the_event) 
	{ 
		return vector<shared_ptr<Event>>();	
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
		Logger::Get()->LogThis("SDLGraphicsManager::Initialize()");
		
		// Initialize SDL Video and Audio subsystems
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0)
		{
			Logger::Get()->LogThis(string("SDL could not initialize:") + const_cast<char*>(SDL_GetError()));
			return false;
		}

		// Initialize SDL Image extension/library
		const int imgFlags = IMG_INIT_PNG;
		if( !( IMG_Init( imgFlags ) & imgFlags ) )
		{
			Logger::Get()->LogThis(string("SDL_image could not initialize:") + const_cast<char*>(SDL_GetError()));
			return false;
		}

		// Create the Window
		window = CreateSDLWindow(static_cast<int>(width), static_cast<int>(height), windowTitle);	
		windowSurface = SDL_GetWindowSurface(window);
		windowRenderer = GetSDLRendererFromWindow(window);
		screenHeight = height;
		screenWidth = width;

		// Set drawing colour to Grey background
		SDL_SetRenderDrawColor(windowRenderer, 192,192,192, 0);

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
	/// Create an Graphic Asset from resource XML
	/// </summary>
	std::shared_ptr<Asset> SDLGraphicsManager::CreateAsset(tinyxml2::XMLElement * resourceElement)
	{		
		// Details that we will extract from object XML
		auto isAnimated = false;
		auto numKeyFrames = 12;   // TODO: Should not be hard coded
		auto keyFrameHeight = 64; // TODO: Should not be hard coded
		auto keyFrameWidth = 64;  // TODO: Should not be hard coded
		
		// Each graphic asset has an id
		int uuid; 		

		// Type of asset
		const char* type;	

		// Were the asset is located on disk
		const char* path;	

		// friendly name
		const char* friendlyName;		

		// Which level the graphic object is for
		int level;

		// Extract object details 
		resourceElement->QueryIntAttribute("scene", &level);
		resourceElement->QueryIntAttribute("uid", &uuid);
		resourceElement->QueryStringAttribute("type", &type);
		resourceElement->QueryStringAttribute("filename", &path);
		resourceElement->QueryStringAttribute("name", &friendlyName);
		
		for(auto attribute = resourceElement->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			std::string name = attribute->Name();
			std::string value = attribute->Value();

			if (name == "isAnimated")
			{
				isAnimated = value == "true" ? true : false;
			}

			if(name == "numKeyFrames")
				numKeyFrames = std::atoi(value.c_str());  // NOLINT(cert-err34-c)
			if(name == "keyFrameHeight")
				keyFrameHeight = atoi(value.c_str());  // NOLINT(cert-err34-c)
			if(name == "keyFrameWidth")
				keyFrameWidth = atoi(value.c_str());  // NOLINT(cert-err34-c)
		}


		auto resource = isAnimated
			                        ? std::shared_ptr<GraphicAsset>(new GraphicAsset(uuid, friendlyName, path, type, level, numKeyFrames, keyFrameHeight, keyFrameWidth,  isAnimated))
			                        : std::shared_ptr<GraphicAsset>(new GraphicAsset(uuid, friendlyName, path, type, level, isAnimated));
			
		
		return resource;
	}

	/// <summary>
	/// Clear the output and draw objects on it
	/// </summary>
	void SDLGraphicsManager::ClearAndDraw(std::function<void(SDL_Renderer* renderer)> &drawObjectsFn) const
	{
		// Backup current render color
		SDL_Color oldColour = {0};
		SDL_GetRenderDrawColor(windowRenderer, &oldColour.r, &oldColour.g, &oldColour.b, &oldColour.a);
		
		// Clear
		SDL_RenderClear(windowRenderer);
			// Draw everything
			drawObjectsFn(windowRenderer);
		
		// Set draw colour
		SDL_SetRenderDrawColor(windowRenderer, oldColour.r, oldColour.g, oldColour.b, oldColour.a); // nb: restore whatever the render routine set as the draw color to
		
		// Show what we've drawn
		SDL_RenderPresent(windowRenderer);
	}
	
	/// <summary>
	/// Draws all the actors in the scene
	/// </summary>
	void SDLGraphicsManager::DrawCurrentScene(SceneManager& sceneManager) const
	{
		// local-func
		auto renderAllObjectsFn = static_cast<render_func>([&](SDL_Renderer*)
		{
			const auto &currentScene = sceneManager;
			
			// Draw all objects in the layer
			for (const auto& Layer : currentScene.GetLayers())
			{
				// Only draw visible layers
				if (Layer.visible)
				{
					// Draw objects within the layer
					for (const auto& gameObjectCandidate : Layer.layerObjects)
					{
						if(auto gameObject = gameObjectCandidate.lock())
						{
							// If it is active
							if (gameObject && gameObject->isActive)
							{
								// draw yourself!
								gameObject->Draw(windowRenderer);
							}
						}
					}
				}
			}
		});

		ClearAndDraw(renderAllObjectsFn);	
	}

	/// <summary>
	/// Uninitialize the SDL Graphics manager
	/// </summary>
	SDLGraphicsManager::~SDLGraphicsManager()
	{
		SDL_DestroyRenderer(windowRenderer);
		SDL_DestroyWindow(window);
	    IMG_Quit();
	    SDL_Quit();
		Instance = nullptr;
	}
}