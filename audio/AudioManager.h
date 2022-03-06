#pragma once
#include "AudioResource.h"
#include <memory>
#include <tinyxml2.h>
#include <SDL_mixer.h>

namespace gamelib
{
	class AudioManager
	{
	public:	
		// Creates an audio Resource
		std::shared_ptr<Asset> create_asset(tinyxml2::XMLElement * asset_xml_element, ResourceManager& resource_admin) const;
		void play_music(Mix_Music* as_music);
		static void play_sound(Mix_Chunk* as_fx);
		static std::shared_ptr<audio_resource> to_resource(const std::shared_ptr<Asset>& asset);
	};
}
