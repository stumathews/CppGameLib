#include "AudioAsset.h"
#include <SDL_mixer.h>
#include "resource/ResourceManager.h"
#include "exceptions/base_exception.h"

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Create Audio asset
	/// </summary>
	AudioAsset::AudioAsset(const int uid, string name, const string path, const string type, int scene, ResourceManager& resource_admin) 
		: Asset(uid, name, path, type, scene), resourceManager(resource_admin)
	{
		assetType = AssetType::Audio;
		if (type._Equal("fx"))
		{
			audioAssetType = AudioAssetType::SoundEffect;
		}
		else if (type._Equal("music"))
		{
			audioAssetType = AudioAssetType::Music;
		}
		else
		{
			THROW(99, string("Unknown Audio Asset sub type: ") + type, "Audio Asset");
		}
	}

	/// <summary>
	/// Return sound effect type
	/// </summary>
	Mix_Chunk* AudioAsset::AsSoundEffect() const
	{ 
		return soundEffect;
	}

	/// <summary>
	/// Return music type
	/// </summary>
	Mix_Music* AudioAsset::AsMusic() const
	{ 
		return music; 
	}

	/// <summary>
	/// Load audio asset into memory
	/// </summary>
	void AudioAsset::Load()
	{
		if (audioAssetType == AudioAssetType::SoundEffect)
		{
			soundEffect = Mix_LoadWAV(resourceManager.GetAssetInfo(name)->path.c_str());
		}
		else if(audioAssetType == AudioAssetType::Music)
		{
			music = Mix_LoadMUS(resourceManager.GetAssetInfo(name)->path.c_str());
		}
		else
		{
			THROW(99, string("Unknown audio asset sub type") + type, "Audio Asset");
		}
		
		isLoadedInMemory = true;
	}

	/// <summary>
	/// Free loaded memory
	/// </summary>
	bool AudioAsset::Unload()
	{
		if(audioAssetType == AudioAssetType::SoundEffect)
		{
			Mix_FreeChunk(soundEffect);
			soundEffect = nullptr;
			isLoadedInMemory = false;
		}
		else 
		{
			if(audioAssetType == AudioAssetType::Music)
			{
				Mix_FreeMusic( music );
				music = nullptr;
				isLoadedInMemory = false;
			}
		}
		return true;
	}
}