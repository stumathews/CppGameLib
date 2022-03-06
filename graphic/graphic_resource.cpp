#include "graphic_resource.h"
#include <SDL_image.h>
#include "sdl_graphics_manager.h"
#include <SDL.h>
#include "common/Common.h"

using namespace std;

namespace gamelib
{
	graphic_resource::graphic_resource(const int uid, std::string name, const std::string& path, const std::string& type, const int level, const bool is_animated, sdl_graphics_manager& graphics_admin, SettingsManager& config)
								 : asset(uid, name, path, type, level),
	                               key_frame_height(64),
	                               key_frame_width(64),
	                               is_animated(is_animated),
								   graphics_admin(graphics_admin),
								   config(config)
	{
		/* ctor initializes member only */
	}
	graphic_resource::graphic_resource(const int uid, std::string name, const std::string& path, const std::string& type, const int level, const uint num_key_frames, const uint key_frame_height, const uint key_frame_width, const bool is_animated, sdl_graphics_manager& graphics_admin, SettingsManager& config)
								 : asset(uid, name, path, type, level),
		                           num_key_frames(num_key_frames),
		                           key_frame_height(key_frame_height),
		                           key_frame_width(key_frame_width),
		                           is_animated(is_animated),
		                           view_port({ 0, 0, static_cast<int>(key_frame_width), static_cast<int>(key_frame_height) }),
								   graphics_admin(graphics_admin),
								   config(config)
	{
		/* ctor initializes member only */
	}
	

	void graphic_resource::load()
	{
		unload();
			  
		const auto loaded_surface = IMG_Load(path.c_str()); //Load image at specified path
		
		if(loaded_surface)
		{						
			texture = SDL_CreateTextureFromSurface(graphics_admin.window_renderer, loaded_surface ); //Create texture from surface pixels
			
			SDL_FreeSurface(loaded_surface); //Get rid of old loaded surface
			
			if(texture)
				is_loaded = true;
		}
		else
		{
			//Logger::log_message(std::string("Unable to load image:") + path + std::string(" Error:") + IMG_GetError());
		}
	}

	bool graphic_resource::unload()
	{		
		SDL_DestroyTexture(texture);
		texture = nullptr;	
		is_loaded = false;
		return true;
	}

	graphic_resource::~graphic_resource()
	{
		/*run_and_log("Destroying graphics resource: " + std::string(name), config.get_bool("global", "verbose"),[&]()
		{
			return graphic_resource::unload();
			
		}, config, true, is_loaded);*/
		graphic_resource::unload();
	}
}