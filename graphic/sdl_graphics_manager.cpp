#include "sdl_graphics_manager.h"
#include <memory>
#include "SDL_image.h"
#include "graphic_resource.h"
#include <iostream>
#include <SDL.h>
#include <functional>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "common/Common.h"
#include "events/event_manager.h"
#include "scene/scene_manager.h"

using namespace std;
namespace gamelib
{

	sdl_graphics_manager::sdl_graphics_manager(shared_ptr<event_manager> event_admin, std::shared_ptr<logger> the_logger)
	: event_subscriber(), event_admin(event_admin), the_logger(std::move(the_logger))
{
		event_admin->subscribe_to_event(event_type::PlayerMovedEventType, this);
	}

	vector<shared_ptr<event>> sdl_graphics_manager::handle_event(const std::shared_ptr<event> the_event)
	{
		if(the_event->type == event_type::PlayerMovedEventType)
		{
			//draw_current_scene();		
		}
		return vector<shared_ptr<event>>();
	}


	sdl_graphics_manager::sdl_graphics_manager(sdl_graphics_manager&& other)
	{
		this->window = other.window;
		this->window_renderer = other.window_renderer;
		this->window_surface = other.window_surface;
		this->screen_height = other.screen_height;
		this->screen_width = other.screen_width;

		other.window = nullptr;
		other.window_renderer = nullptr;
		other.window_surface = nullptr;
		other.screen_height = 0;
		other.screen_width = 0;
	}

	sdl_graphics_manager& sdl_graphics_manager::operator=(sdl_graphics_manager&& other)
	{
		if(&other != this)
		{
			this->window = other.window;
			this->window_renderer = other.window_renderer;
			this->window_surface = other.window_surface;
			this->screen_height = other.screen_height;
			this->screen_width = other.screen_width;

			other.window = nullptr;
			other.window_renderer = nullptr;
			other.window_surface = nullptr;
			other.screen_height = 0;
			other.screen_width = 0;
		}
		return *this;
	}

	string sdl_graphics_manager::get_subscriber_name()
	{
		return "sdl_graphics_manager";
	}

	std::shared_ptr<graphic_resource> sdl_graphics_manager::to_resource(const std::shared_ptr<asset>& asset)
	{
		return as_resource<graphic_resource>(asset);
	}

	SDL_Window* get_sdl_window(const int screen_width, const int screen_height, const char* title)
	{
		const auto out_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height,SDL_WINDOW_SHOWN);	

		if(out_window == nullptr)
			std::cout << "Window could not be created:" << const_cast<char*>(SDL_GetError()) << std::endl;
		
		return out_window;
	}

	SDL_Renderer* get_sdl_window_renderer(SDL_Window* window)
	{
		const auto window_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		
		if(window_renderer == nullptr)
			std::cout << "Renderer could not be created: " << const_cast<char*>(SDL_GetError()) << std::endl;
		
		return window_renderer;
	}


	bool sdl_graphics_manager::initialize(const uint width, const uint height, const char * window_title)
	{
		the_logger->log_message("sdl_graphics_manager::Initialize()");
		
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0)
		{
			the_logger->log_message(string("SDL could not initialize:") + const_cast<char*>(SDL_GetError()));
			return false;
		}

		const int imgFlags = IMG_INIT_PNG;
		if( !( IMG_Init( imgFlags ) & imgFlags ) )
		{
			the_logger->log_message(string("SDL_image could not initialize:") + const_cast<char*>(SDL_GetError()));
			return false;
		}

		window = get_sdl_window(static_cast<int>(width), static_cast<int>(height), window_title);	
		window_surface = SDL_GetWindowSurface(window);
		window_renderer = get_sdl_window_renderer(window);
		screen_height = height;
		screen_width = width;

		// Grey background
		SDL_SetRenderDrawColor(window_renderer, 192,192,192, 0);

		if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 )
	    {
		    const string message("SDL_mixer could not initialize! SDL_mixer Error: ");
			log_message(message + Mix_GetError());
	        return false;
	    }

		if(TTF_Init() == -1)
		{
			const string message("Could not initialize TTF");
			log_message(message);
			return false;
		}


		the_logger->log_message("sdl_graphics_manager ready.");
		return true;
	}

	std::shared_ptr<asset> sdl_graphics_manager::create_asset(tinyxml2::XMLElement * element, std::shared_ptr<settings_manager> config)
	{		
		auto is_animated = false;
		auto num_key_frames = 12, key_frame_height = 64, key_frame_width = 64;

		int uuid; 
		element->QueryIntAttribute("uid", &uuid);
		const char* type;
		element->QueryStringAttribute("type", &type);
		const char* path;
		element->QueryStringAttribute("filename", &path);
		const char* name_c;
		element->QueryStringAttribute("name", &name_c);
		int level;
		element->QueryIntAttribute("scene", &level);
		
		for(auto attribute = element->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			std::string name = attribute->Name();
			std::string value = attribute->Value();

			if(name == "isAnimated")
				is_animated = value == "true" ? true : false;
			if(name == "numKeyFrames")
				num_key_frames = std::atoi(value.c_str());  // NOLINT(cert-err34-c)
			if(name == "keyFrameHeight")
				key_frame_height = atoi(value.c_str());  // NOLINT(cert-err34-c)
			if(name == "keyFrameWidth")
				key_frame_width = atoi(value.c_str());  // NOLINT(cert-err34-c)
		}


		auto resource = is_animated
			                        ? std::make_shared<graphic_resource>(uuid, name_c, path, type, level, num_key_frames, key_frame_height, key_frame_width,  is_animated, shared_from_this(), config)
			                        : std::make_shared<graphic_resource>(uuid, name_c, path, type, level, is_animated, shared_from_this(), config);
			
		
		return resource;
	}

	void sdl_graphics_manager::clear_draw_present(std::function<void(SDL_Renderer* renderer)> &render_routine) const
	{
		// backup current render color
		Uint8 r, g, b, a;
		SDL_GetRenderDrawColor(window_renderer, &r, &g, &b, &a);
		
		SDL_RenderClear(window_renderer);
			render_routine(window_renderer);
			SDL_SetRenderDrawColor(window_renderer, r, g, b, a); // nb: restore whatever the render routine set as the draw color to
		SDL_RenderPresent(window_renderer);
	}

	// Draws all the actors in the scene
	void sdl_graphics_manager::draw_current_scene(std::shared_ptr<scene_manager> scene_admin) const
	{
		auto render_all_objects = static_cast<render_func>([&](SDL_Renderer*)
		{
			const auto &current_scene = scene_admin;
			
			// Draw all objects in the layer
			for (const auto& layer : current_scene->get_scene_layers())
			{
				if (layer->visible)
				{
					for (const auto& game_object : layer->game_objects)
					{
						game_object->draw(window_renderer);
					}
				}
			}
		});

		clear_draw_present(render_all_objects);	
	}


	sdl_graphics_manager::~sdl_graphics_manager()
	{
		SDL_DestroyRenderer(window_renderer);
		SDL_DestroyWindow(window);
	    IMG_Quit();
	    SDL_Quit();
	}
}