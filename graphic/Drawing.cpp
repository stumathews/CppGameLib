#include "Drawing.h"
#include <SDL.h>
#include <iostream>
#include <SDL_image.h>

namespace gamelib
{
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
}