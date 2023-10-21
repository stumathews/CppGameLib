#pragma once
#include <SDL.h>

namespace gamelib
{
	class Drawing
	{
	public:
		Drawing() = default;
		Drawing(const Drawing& other) = delete;
		Drawing(const Drawing&& other) = delete;
		Drawing& operator=(const Drawing& other) = delete;
		Drawing& operator=(const Drawing&& other) = delete;
		~Drawing() = default;
		static void DrawRectangle(int x, int y, int w, int h, SDL_Renderer *onRenderer, int r, int g, int b);
		
	};
}