#include "AudioManager.h"
#include <memory>
#include "tinyxml2.h"
#include "common/Common.h"
#include "asset/asset.h"
#include "exceptions/EngineException.h"

using namespace std;

namespace gamelib
{
	void AudioManager::Play(const shared_ptr<AudioAsset>& asset)
	{
		if(asset->type == "music")
		{
			PlaySdlMusic(asset->AsMusic());
			return;
		}
		if(asset->type == "fx")
		{
			PlaySdlSound(asset->AsSoundEffect());
			return;
		}

		std::stringstream message;
		message << "Unable to Play asset type: " << asset->type << ", Filepath: " << asset->FilePath << std::endl;
		LogMessage(message.str());
	}

	shared_ptr<Asset> AudioManager::CreateAsset(const tinyxml2::XMLElement * assetXmlElement, ResourceManager& resourceManager)
	{
		int uuid;
		const char* type;
		const char* path;
		const char* name;
		int scene;

		assetXmlElement->QueryIntAttribute("uid", &uuid); // 8
		assetXmlElement->QueryStringAttribute("type", &type); // fx, music
		assetXmlElement->QueryStringAttribute("filename", &path); // path\to\sound.wav
		assetXmlElement->QueryStringAttribute("name", &name); // Level1Sound
		assetXmlElement->QueryIntAttribute("scene", &scene); // 0, 1, 2

		// ... Read anything specific to audio in the element here...	
		
		auto audio = std::make_shared<AudioAsset>(uuid, string(name), string(path), string(type), scene,
		                                          resourceManager);

		return audio;
	}

	void AudioManager::Play(const shared_ptr<Asset>& asset)
	{
		Play(ToAudioAsset(asset));
	}
	
	void AudioManager::PlaySdlSound(Mix_Chunk* soundEffect)
	{
		Mix_PlayChannel(-1, soundEffect, 0);
	}

	void AudioManager::PlaySdlMusic(Mix_Music* music)
	{
		Mix_PlayMusic(music, -1);
	}

	/// <summary>
	/// Cast to Audio asset
	/// </summary>
	/// <param name="asset"></param>
	/// <returns></returns>
	shared_ptr<AudioAsset> AudioManager::ToAudioAsset(const shared_ptr<Asset>& asset)
	{	
		if (asset->AssetType != Asset::AssetType::Audio)
		{
			// TODO: Fix
			THROW(97, "Cannot cast a generic asset that is not an audio asset to an audio asset", "Audio Manager")
		}

		return AsAsset<AudioAsset>(asset);
	}

	AudioManager* AudioManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new AudioManager();
		}
		return instance;
	}

	AudioManager::~AudioManager()
	{
		instance = nullptr;
	}	

	AudioManager* AudioManager::instance = nullptr;

}