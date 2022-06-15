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

		/// <summary>
		/// Creates an Audio Asset
		/// </summary>
		std::shared_ptr<Asset> CreateAsset(tinyxml2::XMLElement * assetXmlElement, ResourceManager& resourceManager) const;
		
		/// <summary>
		/// Play specific music asset
		/// </summary>
		/// <param name="music"></param>
		void Play(Mix_Music* music);

		/// <summary>
		/// Plays sound effect
		/// </summary>
		static void Play(Mix_Chunk* soundEffect);

		/// <summary>
		/// Cast base class to Specific Audio Asset
		/// </summary>
		static std::shared_ptr<AudioAsset> ToAudioAsset(const std::shared_ptr<Asset>& asset);
	protected:
		static AudioManager* Instance;
	private:
		AudioManager() = default;
		
	};
}
