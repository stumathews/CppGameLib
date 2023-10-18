#pragma once
#include "AudioAsset.h"
#include <memory>
#include <tinyxml2.h>
#include <SDL_mixer.h>

namespace gamelib
{
	class AudioManager
	{
	public:		

		static AudioManager* Get();
		~AudioManager();
		AudioManager(const AudioManager& other) = delete;
		AudioManager(const AudioManager&& other) = delete;
		AudioManager& operator=(const AudioManager& other) = delete;
		AudioManager& operator=(const AudioManager&& other) = delete;


		// Creates an Audio Asset
		static std::shared_ptr<Asset> CreateAsset(const tinyxml2::XMLElement * assetXmlElement, ResourceManager& resourceManager);		
		
		static void Play(const std::shared_ptr<AudioAsset>& asset);
		static void Play(const std::shared_ptr<Asset>& asset);
		static std::shared_ptr<AudioAsset> ToAudioAsset(const std::shared_ptr<Asset>& asset);
	protected:
		static AudioManager* instance;
		static void PlaySdlMusic(Mix_Music* music);
		static void PlaySdlSound(Mix_Chunk* soundEffect);
	private:
		AudioManager() = default;
		
	};
}
