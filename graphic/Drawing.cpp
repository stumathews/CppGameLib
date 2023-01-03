#include "Drawing.h"
#include <SDL.h>
#include <iostream>
#include <SDL_image.h>

namespace gamelib
{
	Drawing::Drawing() 	= default;

	void Drawing::DrawRectangle(const int x, const int y, const int w, const int h, SDL_Renderer* onRenderer,
	                            const int r, const int g, const int b)
	{

		// Define a rectangle
		const SDL_Rect fillRect = { x == 0 ? 3 : x , y == 0 ? 4 : y , w, h };
		
		// Set draw colour on renderer
		SDL_SetRenderDrawColor(onRenderer, static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 0xFF);

		// Send to renderer
		SDL_RenderFillRect(onRenderer, &fillRect);
	}


	Drawing::~Drawing() = default;


	SDL_Texture* MakeTexture(const char* texturePath)
	{
		SDL_Texture* newTexture = nullptr;
		SDL_Surface* imageSurface = IMG_Load(texturePath);

		//Get rid of old loaded surface
		SDL_FreeSurface(imageSurface);
		return newTexture;
	}


	

	SDL_Texture* TryMakeTexture(const char* path)
	{	
		if(path == nullptr)
		{
			std::cout << "Texture path cant be empty!" << std::endl;
		}

		SDL_Texture* outTexture = MakeTexture(path);	

		if(outTexture == nullptr)
		{
			std::cout << "Could not load textture" << std::endl;
		}
		return outTexture;
	}

	void RenderLine(SDL_Renderer* toRenderer, const unsigned int screenHeight, const unsigned int screenWidth)
	{
		 //Draw blue horizontal line
		SDL_SetRenderDrawColor( toRenderer, 0x00, 0x00, 0xFF, 0xFF );
		SDL_RenderDrawLine( toRenderer, 0, static_cast<int>(screenHeight / 2), static_cast<int>(screenWidth), static_cast<int>(screenHeight / 2) );
	}
}