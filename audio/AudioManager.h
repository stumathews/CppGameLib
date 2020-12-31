#pragma once
#include "AudioResource.h"
#include <memory>
#include <tinyxml2.h>
#include <SDL_mixer.h>

#include "events/event_subscriber.h"

namespace gamelib
{
	class audio_manager
	{
	public:	
		// Creates an audio Resource
		std::shared_ptr<asset> create_asset(tinyxml2::XMLElement * asset_xml_element, std::shared_ptr<resource_manager> resource_admin) const;
		void play_music(Mix_Music* as_music);
		static void play_sound(Mix_Chunk* as_fx);
		static std::shared_ptr<audio_resource> to_resource(const std::shared_ptr<asset>& asset);
	};
}
