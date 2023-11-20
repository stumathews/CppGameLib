#include "GraphicAsset.h"
#include <SDL_image.h>
#include "SDLGraphicsManager.h"
#include <SDL.h>
#include "common/Common.h"
#include <file/Logger.h>

using namespace std;

namespace gamelib
{
	GraphicAsset::GraphicAsset(const int inUid, const std::string& inName, const std::string& inPath, const std::string& inType, 
		const int level, const AbcdRectangle& dimensions)
		: Asset(inUid, inName, inPath, inType, level), Dimensions(dimensions)
	{
		hasColourKey = false;
		viewPort = { dimensions.GetAx(), dimensions.GetAy(), dimensions.GetWidth(), dimensions.GetHeight() };
		AssetType = AssetType::Graphic;
	}

	// Load into memory
	void GraphicAsset::Load()
	{
		Unload();
			  
		// Load image at specified path

		if(const auto loadedSurface = IMG_Load(FilePath.c_str()))
		{				
			if (HasColourKey())
			{
				SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, static_cast<Uint8>(colourKey.Red), static_cast<Uint8>(colourKey.Green), static_cast<Uint8>(colourKey.Blue)));
			}

			// Create texture from surface pixels
			texture = SDL_CreateTextureFromSurface(SdlGraphicsManager::Get()->WindowRenderer, loadedSurface);
						
			// Get rid of old loaded surface (we have the texture pixels)
			SDL_FreeSurface(loadedSurface);
			
			// Mark the asset as having been loaded
			if (texture)
			{
				IsLoadedInMemory = true;
			}
		}
		else
		{
			Logger::Get()->LogThis(std::string("Unable to load image:") + FilePath + std::string(" Error:") + IMG_GetError());
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
			IsLoadedInMemory = false;
			
			isSuccess = true;
		} 
		catch (...)
		{
			Logger::Get()->LogThis(string("Unable to Unload asset: " + this->Name) + string(", path:" + this->FilePath));
			isSuccess = false;
		}

		return isSuccess;
	}

	/// <summary>
	/// Get Observable area of the graphic
	/// </summary>
	/// <returns></returns>
	const SDL_Rect& GraphicAsset::GetViewPort() const
	{
		return viewPort;
	}
		
	SDL_Texture* GraphicAsset::GetTexture() const
	{
		return texture;
	}

	void GraphicAsset::SetColourKey(int red, int green, int blue)
	{
		colourKey = { red, green, blue };
		hasColourKey = true;
	}

	ColourKey GraphicAsset::GetColourKey() const
	{
		return colourKey;
	}

	bool GraphicAsset::HasColourKey() const
	{
		return hasColourKey;
	}

	GraphicAsset::~GraphicAsset()
	{
		// Unload the asset if if not longer be used/referenced anywhere/more
		
		Logger::Get()->LogThis(string("Unloading asset: " + this->FilePath));

		GraphicAsset::Unload();
	}
}