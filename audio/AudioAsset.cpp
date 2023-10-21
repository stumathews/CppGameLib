#include "AudioAsset.h"
#include <SDL_mixer.h>
#include "resource/ResourceManager.h"
#include "exceptions/EngineException.h"

using namespace std;

namespace gamelib
{
	AudioAsset::AudioAsset(const int inUid, const string& inName, const string& inPath, const string& inAudioAssetType,
	                       const int inScene,
	                       ResourceManager& resourceAdmin)
		: Asset(inUid, inName, inPath, inAudioAssetType, inScene), resourceManager(resourceAdmin)
	{
		AssetType = AssetType::Audio;

		if (inAudioAssetType == "fx")
		{
			audioAssetType = AudioAssetType::SoundEffect;
		}
		else if (inAudioAssetType == "music")
		{
			audioAssetType = AudioAssetType::Music;
		}
		else
		{
			// TODO: Fix
			THROW(99, string("Unknown Audio Asset sub type: ") + inAudioAssetType, "Audio Asset")
		}
	}

	Mix_Chunk* AudioAsset::AsSoundEffect() const
	{
		return SoundEffect;
	}

	Mix_Music* AudioAsset::AsMusic() const
	{
		return Music;
	}

	void AudioAsset::Load()
	{
		if (audioAssetType == AudioAssetType::SoundEffect)
		{
			SoundEffect = Mix_LoadWAV(resourceManager.GetAssetInfo(Name)->FilePath.c_str());
		}
		else if (audioAssetType == AudioAssetType::Music)
		{
			Music = Mix_LoadMUS(resourceManager.GetAssetInfo(Name)->FilePath.c_str());
		}
		else
		{
			// TODO: Fix
			THROW(99, string("Unknown audio asset sub type") + Type, "Audio Asset")
		}

		IsLoadedInMemory = true;
	}

	bool AudioAsset::Unload()
	{
		if (audioAssetType == AudioAssetType::SoundEffect)
		{
			Mix_FreeChunk(SoundEffect);
			SoundEffect = nullptr;
			IsLoadedInMemory = false;
		}
		else
		{
			if (audioAssetType == AudioAssetType::Music)
			{
				Mix_FreeMusic(Music);
				Music = nullptr;
				IsLoadedInMemory = false;
			}
		}
		return true;
	}
}
