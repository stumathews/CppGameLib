#include "AudioResource.h"
#include <SDL_mixer.h>
#include "resource/resource_manager.h"

using namespace std;

namespace gamelib
{
	//extern std::shared_ptr<resource_manager> resource_admin;
	audio_resource::audio_resource(const int uid, string name, const string path, const string type, int scene, shared_ptr<resource_manager> resource_admin) 
		: asset(uid, name, path, type, scene), resource_admin(resource_admin)
	{
		// ctor initialize members only
	}

	void audio_resource::load()
	{
		if(is_fx())
			fx = Mix_LoadWAV(resource_admin->get(name)->path.c_str());
		else
			music = Mix_LoadMUS(resource_admin->get(name)->path.c_str());
		
		is_loaded = true;
	}

	bool audio_resource::is_fx() const
	{
		return type == "fx";
	}


	bool audio_resource::unload()
	{
		if(is_fx() && fx != nullptr)
		{
			Mix_FreeChunk(fx);
			fx = nullptr;
			is_loaded = false;
		}
		else 
		{
			if(music != nullptr)
			{
				 Mix_FreeMusic( music );
				music = nullptr;
				is_loaded = false;
			}
		}
		return true;
	}

}