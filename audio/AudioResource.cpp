#include "AudioResource.h"
#include <SDL_mixer.h>
#include "resource/ResourceManager.h"

using namespace std;

namespace gamelib
{
	//extern std::ResourceManager& resource_admin;
	audio_resource::audio_resource(const int uid, string name, const string path, const string type, int scene, ResourceManager& resource_admin) 
		: Asset(uid, name, path, type, scene), resource_admin(resource_admin)
	{
		// ctor initialize members only
	}

	void audio_resource::Load()
	{
		if(is_fx())
			fx = Mix_LoadWAV(resource_admin.GetAssetInfo(name)->path.c_str());
		else
			music = Mix_LoadMUS(resource_admin.GetAssetInfo(name)->path.c_str());
		
		isLoadedInMemory = true;
	}

	bool audio_resource::is_fx() const
	{
		return type == "fx";
	}


	bool audio_resource::Unload()
	{
		if(is_fx() && fx != nullptr)
		{
			Mix_FreeChunk(fx);
			fx = nullptr;
			isLoadedInMemory = false;
		}
		else 
		{
			if(music != nullptr)
			{
				 Mix_FreeMusic( music );
				music = nullptr;
				isLoadedInMemory = false;
			}
		}
		return true;
	}

}