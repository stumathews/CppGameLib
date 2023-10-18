#pragma once
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
		AudioAsset(int inUid, const std::string& inName, const std::string& inPath, const std::string& inAudioAssetType, int inScene, ResourceManager& resourceAdmin);

		/// <summary>
		/// Sound effect data 
		/// </summary>
		Mix_Chunk *SoundEffect = nullptr;	

		/// <summary>
		/// Music data 
		/// </summary>
		Mix_Music* Music = nullptr;

		/// <summary>
		/// Gets Sound effect data 
		/// </summary>
		[[nodiscard]] Mix_Chunk* AsSoundEffect() const;

		/// <summary>
		/// Get Music data
		/// </summary>
		[[nodiscard]] Mix_Music* AsMusic() const;

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

