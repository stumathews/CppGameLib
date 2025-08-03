#pragma once
#include <SDL.h>
#include "asset/asset.h"
#include "geometry/ABCDRectangle.h"
#include "ColourKey.h"

namespace gamelib
{
	/// <summary>
	/// A stand-alone graphics resource with backing storage that can load and unload itself
	/// </summary>
	class GraphicAsset : public Asset
	{
	public:	
		/// <summary>
		/// Create A Graphic Asset
		/// </summary>
		GraphicAsset(int inUid, 
		             const std::string& inName, 
		             const std::string& inPath, 
		             const std::string& inType,
		             int level, 
		             const AbcdRectangle& dimensions);

		~GraphicAsset() override;		

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
		[[nodiscard]] const SDL_Rect& GetViewPort() const;
		
		/// <summary>
		/// Gets the graphic's texture
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] SDL_Texture* GetTexture() const;
				
		AbcdRectangle Dimensions;

		void SetColourKey(int red, int green, int blue);

		[[nodiscard]] ColourKey GetColourKey() const;

		[[nodiscard]] bool HasColourKey() const;

	private:
	
		bool hasColourKey;

		ColourKey colourKey;

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

