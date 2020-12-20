#pragma once
#include <SDL.h>

namespace gamelib
{
	class Drawing
	{
	public:
		Drawing();
		static void DrawRectangle(const int m_xPos, const int m_yPos, const int w, const int h, SDL_Renderer *onRenderer, int r, int g, int b);
		
		~Drawing();
	};
}