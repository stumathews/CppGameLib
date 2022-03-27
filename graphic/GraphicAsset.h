#pragma once
#include <SDL.h>
#include "asset/asset.h"
#include "common/aliases.h"
#include "util/SettingsManager.h"
#include <list>
#include <KeyFrame.h>

namespace gamelib
{
	class SDLGraphicsManager;
	class Logger;

	/// <summary>
	/// A stand-alone graphics resource with backing storage that can load and unload itselfe
	/// </summary>
	class GraphicAsset : public Asset
	{
	public:		

		/// <summary>
		/// Create A Graphic Asset
		/// </summary>
		GraphicAsset(const int uid, 
			std::string name, 
			const std::string& path, 
			const std::string& type, 
			const int level, 
			const bool is_animated);

		/// <summary>
		/// Create A Graphic Asset
		/// </summary>
		GraphicAsset(const int uid, 
			std::string name, 
			const std::string& path, 
			const std::string& type, 
			const int level, 
			const uint num_key_frames, 
			uint key_frame_height, 
			const uint key_frame_width,
			const bool is_animated);

		virtual ~GraphicAsset();		

		/// <summary>
		/// Load the resource onto the surface
		/// </summary>
		void Load() override;

		/// <summary>
		/// a Resource can unload itself from memory
		/// </summary>
		/// <returns></returns>
		bool Unload() override;

		/// <summary>
		/// Get Observable area of the graphic
		/// </summary>
		/// <returns></returns>
		SDL_Rect& GetViewPort();

		/// <summary>
		/// Determines if the graphic is animated
		/// </summary>
		/// <returns></returns>
		bool IsAnimated() const;
		
		/// <summary>
		/// Gets the graphic's texture
		/// </summary>
		/// <returns></returns>
		SDL_Texture* GetTexture() const;

		uint GetNumKeyFrames() const;

		uint GetKeyFrameHeight() const;

		uint GetKeyFrameWidth() const;

		std::list<KeyFrame> KeyFrames;

	private:
		
		/// <summary>
		/// The binary data that will represent the resource once its loaded.
		/// </summary>
		SDL_Texture* texture = nullptr;

		uint numKeyFrames = 0;
		uint keyFrameHeight = 0;
		uint keyFrameWidth = 0;
		bool isAnimated = false;

		/// <summary>
		/// Observable area of the graphic
		/// </summary>
		SDL_Rect viewPort = {};
	};
}

