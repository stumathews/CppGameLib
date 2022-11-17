#pragma once
#include <SDL.h>
#include "asset/asset.h"
#include "common/aliases.h"
#include "util/SettingsManager.h"
#include <list>
#include <KeyFrame.h>
#include <Inventory.h>
#include "scene/ABCDRectangle.h"
#include "ColourKey.h"

namespace gamelib
{
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
			const std::string name, 
			const std::string& path, 
			const std::string& type, 
			const int level, 
			const ABCDRectangle& dimensions);

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
		/// Gets the graphic's texture
		/// </summary>
		/// <returns></returns>
		SDL_Texture* GetTexture() const;
				
		std::list<KeyFrame> KeyFrames;

		Inventory properties;

		ABCDRectangle Dimensions;

		void SetColourKey(int red, int green, int blue);

		ColourKey GetColourKey();

		bool HasColourKey();

	private:
	
		bool hasColourKey;

		ColourKey ColourKey;

		/// <summary>
		/// The binary data that will represent the resource once its loaded.
		/// </summary>
		SDL_Texture* texture = nullptr;

		/// <summary>
		/// Observable area of the graphic
		/// </summary>
		SDL_Rect viewPort = {};
	};
}

