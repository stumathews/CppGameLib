#include "AudioAsset.h"
#include <SDL_mixer.h>
#include "resource/ResourceManager.h"
#include "exceptions/EngineException.h"

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Create Audio asset
	/// </summary>
	AudioAsset::AudioAsset(const int inUid, const string& inName, const string& inPath, const string& inType, const int inScene,
	                       ResourceManager& resourceAdmin) 
		: Asset(inUid, inName, inPath, inType, inScene), resourceManager(resourceAdmin)
	{
		assetType = AssetType::Audio;
		if (inType._Equal("fx"))
		{
			audioAssetType = AudioAssetType::SoundEffect;
		}
		else if (inType._Equal("music"))
		{
			audioAssetType = AudioAssetType::Music;
		}
		else
		{
			THROW(99, string("Unknown Audio Asset sub type: ") + inType, "Audio Asset")
		}
	}

	/// <summary>
	/// Return sound effect type
	/// </summary>
	Mix_Chunk* AudioAsset::AsSoundEffect() const
	{ 
		return SoundEffect;
	}

	/// <summary>
	/// Return music type
	/// </summary>
	Mix_Music* AudioAsset::AsMusic() const
	{ 
		return Music; 
	}

	/// <summary>
	/// Load audio asset into memory
	/// </summary>
	void AudioAsset::Load()
	{
		if (audioAssetType == AudioAssetType::SoundEffect)
		{
			SoundEffect = Mix_LoadWAV(resourceManager.GetAssetInfo(name)->path.c_str());
		}
		else if(audioAssetType == AudioAssetType::Music)
		{
			Music = Mix_LoadMUS(resourceManager.GetAssetInfo(name)->path.c_str());
		}
		else
		{
			THROW(99, string("Unknown audio asset sub type") + type, "Audio Asset")
		}
		
		IsLoadedInMemory = true;
	}

	/// <summary>
	/// Free loaded memory
	/// </summary>
	bool AudioAsset::Unload()
	{
		if(audioAssetType == AudioAssetType::SoundEffect)
		{
			Mix_FreeChunk(SoundEffect);
			SoundEffect = nullptr;
			IsLoadedInMemory = false;
		}
		else 
		{
			if(audioAssetType == AudioAssetType::Music)
			{
				Mix_FreeMusic( Music );
				Music = nullptr;
				IsLoadedInMemory = false;
			}
		}
		return true;
	}
}