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
	shared_ptr<Asset> AudioManager::CreateAsset(tinyxml2::XMLElement * element, ResourceManager& resource_admin) const
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
		
		auto audio = shared_ptr<AudioAsset>(new AudioAsset(uuid, string(name), string(path), string(type), scene, resource_admin));

		return audio;
	}

	/// <summary>
	/// Play Music
	/// </summary>
	/// <param name="as_music"></param>
	void AudioManager::Play(Mix_Music* as_music)
	{
		Mix_PlayMusic(as_music, -1);
	}

	/// <summary>
	/// Play sound
	/// </summary>
	/// <param name="as_fx"></param>
	void AudioManager::Play(Mix_Chunk* as_fx)
	{
		Mix_PlayChannel(-1, as_fx, 0);
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
			THROW(97, "Cannot cast a generic asset that is not an audio asset to an audio asset", "Audio Manager");
		}

		return AsAsset<AudioAsset>(asset);
	}

}