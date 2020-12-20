#pragma once
#include <memory>
#include <string>
#include <SDL_mixer.h>
#include "asset/asset.h"

namespace gamelib
{
	class resource_manager;

	class audio_resource final : public asset
	{
		std::shared_ptr<resource_manager> resource_admin;
	public:
		audio_resource(int uid, std::string name, const std::string path, const std::string type, int scene, std::shared_ptr<resource_manager> resource_admin);

		// data
		Mix_Chunk *fx = nullptr;	
		Mix_Music* music = nullptr;

		Mix_Chunk *as_fx() const { return fx;}
		Mix_Music *as_music() const {return music;}

		
		//Load the resource onto the surface
		void load() override;
		bool is_fx() const;

		// a render resource can unload itself from memory
		bool unload() override;
	};
}

