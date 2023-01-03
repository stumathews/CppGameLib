#include "AudioManager.h"
#include <memory>
#include "tinyxml2.h"
#include "common/Common.h"
#include "asset/asset.h"
#include "exceptions/EngineException.h"

using namespace std;

namespace gamelib
{

	AudioManager* AudioManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new AudioManager();
		}
		return Instance;
	}

	AudioManager::~AudioManager()
	{
		Instance = nullptr;
	}

	AudioManager* AudioManager::Instance = nullptr;

	/// <summary>
	/// Create Audio Asset
	/// </summary>
	shared_ptr<Asset> AudioManager::CreateAsset(const tinyxml2::XMLElement * assetXmlElement, ResourceManager& resourceManager)
	{
		int uuid;
		const char* type;
		const char* path;
		const char* name;
		int scene;

		assetXmlElement->QueryIntAttribute("uid", &uuid);
		assetXmlElement->QueryStringAttribute("type", &type);
		assetXmlElement->QueryStringAttribute("filename", &path);
		assetXmlElement->QueryStringAttribute("name", &name);
		assetXmlElement->QueryIntAttribute("scene", &scene);

		// ... Read anything specific to audio in the element here...	
		
		auto audio = std::make_shared<AudioAsset>(uuid, string(name), string(path), string(type), scene,
		                                          resourceManager);

		return audio;
	}

	/// <summary>
	/// Play Music
	/// </summary>
	/// <param name="music"></param>
	void AudioManager::Play(Mix_Music* music)
	{
		Mix_PlayMusic(music, -1);
	}

	/// <summary>
	/// Play sound
	/// </summary>
	/// <param name="soundEffect"></param>
	void AudioManager::Play(Mix_Chunk* soundEffect)
	{
		Mix_PlayChannel(-1, soundEffect, 0);
	}

	/// <summary>
	/// Cast to Audio asset
	/// </summary>
	/// <param name="asset"></param>
	/// <returns></returns>
	shared_ptr<AudioAsset> AudioManager::ToAudioAsset(const shared_ptr<Asset>& asset)
	{	
		if (asset->assetType != Asset::AssetType::Audio)
		{
			THROW(97, "Cannot cast a generic asset that is not an audio asset to an audio asset", "Audio Manager")
		}

		return AsAsset<AudioAsset>(asset);
	}

}