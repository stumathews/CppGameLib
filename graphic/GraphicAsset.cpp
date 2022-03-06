#include "GraphicAsset.h"
#include <SDL_image.h>
#include "SDLGraphicsManager.h"
#include <SDL.h>
#include "common/Common.h"
#include <common/Logger.h>

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Create Graphic Asset
	/// </summary>
	GraphicAsset::GraphicAsset(const int uid, 
		std::string name, 
		const std::string& path, 
		const std::string& type,
		const int level, 
		const bool isAnimated, 
		SDLGraphicsManager& graphicsManager, 
		SettingsManager& config,
		Logger& logger)
								 : Asset(uid, name, path, type, level),
	                               keyFrameHeight(64),
	                               keyFrameWidth(64),
	                               isAnimated(isAnimated),
								   graphicsManager(graphicsManager),
								   settingsManager(config),
								   logger(logger){ }

	/// <summary>
	/// Create Graphic Asset
	/// </summary>
	GraphicAsset::GraphicAsset(const int uid, 
		std::string name, 
		const std::string& path, 
		const std::string& type, 
		const int level, 
		const uint numKeyFrames, 
		const uint keyFrameHeight, 
		const uint keyFrameWidth, 
		const bool isAnimated, 
		SDLGraphicsManager& graphicsManager, 
		SettingsManager& config,
		Logger& logger)
								 : Asset(uid, name, path, type, level),
		                           numKeyFrames(numKeyFrames),
		                           keyFrameHeight(keyFrameHeight),
		                           keyFrameWidth(keyFrameWidth),
		                           isAnimated(isAnimated),
		                           viewPort({ 0, 0, static_cast<int>(keyFrameWidth), static_cast<int>(keyFrameHeight) }),
								   graphicsManager(graphicsManager),
								   settingsManager(config), 
		logger(logger) { }
	
	/// <summary>
	/// Load asset into memory
	/// </summary>
	void GraphicAsset::Load()
	{
		Unload();
			  
		// Load image at specified path
		const auto loadedSurface = IMG_Load(path.c_str());
		
		if(loadedSurface)
		{						
			// Create texture from surface pixels
			texture = SDL_CreateTextureFromSurface(graphicsManager.window_renderer, loadedSurface );
			
			// Get rid of old loaded surface (we have the texture pixels)
			SDL_FreeSurface(loadedSurface);
			
			// Mark the asset as having been loaded
			if (texture)
			{
				isLoadedInMemory = true;
			}
		}
		else
		{
			logger.LogThis(std::string("Unable to load image:") + path + std::string(" Error:") + IMG_GetError());
		}
	}

	/// <summary>
	/// Unload asset from memory
	/// </summary>
	/// <returns>true if successfully unloaded asset, false otherwise</returns>
	bool GraphicAsset::Unload()
	{	
		auto isSuccess = false;
		try
		{
			// Free texture in memory
			SDL_DestroyTexture(texture);

			// make it clear that these are not used now
			texture = nullptr;
			isLoadedInMemory = false;
			
			isSuccess = true;
		} 
		catch (...)
		{
			logger.LogThis(string("Unable to Unload asset: " + this->name) + string(", path:" + this->path));
			isSuccess = false;
		}

		return isSuccess;
	}

	/// <summary>
	/// Get Observable area of the graphic
	/// </summary>
	/// <returns></returns>
	SDL_Rect& GraphicAsset::GetViewPort()
	{
		return viewPort;
	}

	bool GraphicAsset::IsAnimated() const
	{
		return isAnimated;
	}

	SDL_Texture* GraphicAsset::GetTexture() const
	{
		return texture;
	}

	uint GraphicAsset::GetNumKeyFrames() const
	{
		return numKeyFrames;
	}

	uint GraphicAsset::GetKeyFrameHeight() const
	{
		return keyFrameHeight;
	}

	uint GraphicAsset::GetKeyFrameWidth() const
	{
		return keyFrameWidth;
	}

	GraphicAsset::~GraphicAsset()
	{
		// Unload the asset if if not longer be used/referenced anywhere/more
		
		logger.LogThis(string("Unloading asset: " + this->path));

		Unload();
	}
}