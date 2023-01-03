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
	GraphicAsset::GraphicAsset(const int inUid, const std::string& inName, const std::string& inPath, const std::string& inType, 
		const int level, const ABCDRectangle& dimensions)
		: Asset(inUid, inName, inPath, inType, level), Dimensions(dimensions)
	{
		hasColourKey = false;
		viewPort = { dimensions.GetAx(), dimensions.GetAy(), dimensions.GetWidth(), dimensions.GetHeight() };
		assetType = AssetType::Graphic;
	}
	
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
			if (HasColourKey())
			{
				SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, static_cast<Uint8>(ColourKey.Red), static_cast<Uint8>(ColourKey.Green), static_cast<Uint8>(ColourKey.Blue)));
			}

			// Create texture from surface pixels
			texture = SDL_CreateTextureFromSurface(SDLGraphicsManager::Get()->WindowRenderer, loadedSurface );

			
			
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
			Logger::Get()->LogThis(std::string("Unable to load image:") + path + std::string(" Error:") + IMG_GetError());
		}
	}

	/// <summary>
	/// Unload asset from memory
	/// </summary>
	/// <returns>true if successfully unloaded asset, false otherwise</returns>
	bool GraphicAsset::Unload()
	{	
		bool isSuccess;
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
			Logger::Get()->LogThis(string("Unable to Unload asset: " + this->name) + string(", path:" + this->path));
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
		
	SDL_Texture* GraphicAsset::GetTexture() const
	{
		return texture;
	}

	void GraphicAsset::SetColourKey(int red, int green, int blue)
	{
		ColourKey = { red, green, blue };
		hasColourKey = true;
	}

	ColourKey GraphicAsset::GetColourKey() const
	{
		return ColourKey;
	}

	bool GraphicAsset::HasColourKey() const
	{
		return hasColourKey;
	}

	GraphicAsset::~GraphicAsset()
	{
		// Unload the asset if if not longer be used/referenced anywhere/more
		
		Logger::Get()->LogThis(string("Unloading asset: " + this->path));

		GraphicAsset::Unload();
	}
}