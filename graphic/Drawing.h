#pragma once
#include <SDL.h>

namespace gamelib
{
	class Drawing
	{
	public:
		Drawing();
		static void DrawRectangle(int x, int y, int w, int h, SDL_Renderer *onRenderer, int r, int g, int b);
		
		~Drawing();
	};
}