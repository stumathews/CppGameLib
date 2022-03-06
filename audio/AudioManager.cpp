#include "AudioManager.h"
#include <memory>
#include "tinyxml2.h"
#include "common/Common.h"


using namespace std;
namespace gamelib
{
	std::shared_ptr<asset> AudioManager::create_asset(tinyxml2::XMLElement * element, ResourceManager& resource_admin) const
	{
		int uuid;
		const char* type;
		const char* path;
		const char* name;
		int scene;

		element->QueryIntAttribute("uid", &uuid);
		element->QueryStringAttribute("type", &type);
		element->QueryStringAttribute("filename", &path);
		element->QueryStringAttribute("name", &name);
		element->QueryIntAttribute("scene", &scene);
		// ... Read anything specific to audio in the element here...	
		
		auto audio = std::make_shared<audio_resource>(uuid, std::string(name), string(path), string(type), scene, resource_admin);

		return audio;
	}

	void AudioManager::play_music(Mix_Music* as_music)
	{
		Mix_PlayMusic(as_music, -1);
	}

	void AudioManager::play_sound(Mix_Chunk* as_fx)
	{
		Mix_PlayChannel(-1, as_fx, 0);
	}

	shared_ptr<audio_resource> AudioManager::to_resource(const shared_ptr<asset>& asset)
	{	
		return as_resource<audio_resource>(asset);
	}

}