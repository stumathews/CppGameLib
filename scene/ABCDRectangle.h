#pragma once
#include <SDL.h>
#include "util/Tuple.h"

namespace gamelib
{
	/*
	 An ABCD Rectangle looks like this;

		A----B
		|    |
		|    |
		D----C

	Each point A, B, C, D has and x,y coordinate

		A(ax,ay)----B(bx,by)
		|                  |
		|                  |
		D(dx,dy)----C(cx,cy)
	
	*/
	class ABCDRectangle
	{
	public:
		ABCDRectangle() = default;
		ABCDRectangle(const SDL_Rect rect);
		ABCDRectangle(int x, int y, int w, int h);
		
		int GetAx() const;
		int GetAy() const;
		int GetBx() const;
		int GetBy() const;
		int GetCx() const;
		int GetCy() const;
		int GetDx() const;
		int GetDy() const;

		void SetX(int x);
		void SetY(int y);
		void SetHeight(int h);
		void SetWidth(int w);
		int GetHeight() const;
		int GetWidth() const;

		void reinitialize(int x, int y, int w, int h);

	private:
		int x,y,w,h;
	};

}