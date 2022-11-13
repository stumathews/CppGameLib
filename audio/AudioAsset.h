#pragma once
#include <memory>
#include <string>
#include <SDL_mixer.h>
#include "asset/asset.h"

namespace gamelib
{
	class ResourceManager;

	/// <summary>
	/// Audio Asset
	/// </summary>
	class AudioAsset final : public Asset
	{
	public:

		enum class AudioAssetType
		{
			SoundEffect,
			Music
		};

		/// <summary>
		/// Create an audio asset
		/// </summary>
		AudioAsset(int uid, std::string name, const std::string path, const std::string type, int scene, ResourceManager& resourceAdmin);

		/// <summary>
		/// Sound effect data 
		/// </summary>
		Mix_Chunk *soundEffect = nullptr;	

		/// <summary>
		/// Music data 
		/// </summary>
		Mix_Music* music = nullptr;

		/// <summary>
		/// Gets Sound effect data 
		/// </summary>
		Mix_Chunk* AsSoundEffect() const;

		/// <summary>
		/// Get Music data
		/// </summary>
		Mix_Music* AsMusic() const;

		/// <summary>
		/// Load audio into memory
		/// </summary>
		void Load() override;

		/// <summary>
		/// Free audio data from memory
		/// </summary>
		bool Unload() override;
	private:

		/// <summary>
		/// Resource manager
		/// </summary>
		ResourceManager& resourceManager;

		/// <summary>
		/// Audio assetType
		/// Eg. could be Music or Sound effect
		/// </summary>
		AudioAssetType audioAssetType;
	};
}

