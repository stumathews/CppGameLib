#pragma once
#include <SDL.h>
#include "asset/asset.h"
#include "common/aliases.h"
#include "util/SettingsManager.h"

namespace gamelib
{
	class SDLGraphicsManager;

	// A stand-alone graphics resource with backing storage that can load and unload itself
	class graphic_resource : public asset
	{
		SDLGraphicsManager& graphics_admin;
		SettingsManager& config;
	public:		
		graphic_resource(const int uid, std::string name, const std::string& path, const std::string& type, const int level, const bool is_animated, SDLGraphicsManager& graphics_admin, SettingsManager& config);
		graphic_resource(const int uid, std::string name, const std::string& path, const std::string& type, const int level, const uint num_key_frames, uint key_frame_height, const uint key_frame_width, const bool is_animated, SDLGraphicsManager& graphics_admin, SettingsManager& config);
		virtual ~graphic_resource();

		SDL_Texture* texture = nullptr; // The binary data that will represent the resource once its loaded.

		uint num_key_frames = 0;
		uint key_frame_height = 0;
		uint key_frame_width = 0;
		bool is_animated = false;
		SDL_Rect view_port = {};

		//Load the resource onto the surface
		void load() override;

		// a render resource can unload itself from memory
		bool unload() override;
	};
}

