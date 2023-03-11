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
		ABCDRectangle(SDL_Rect rect);
		ABCDRectangle(int x, int y, int w, int h);

		[[nodiscard]] int GetAx() const;
		[[nodiscard]] int GetAy() const;
		[[nodiscard]] int GetBx() const;
		[[nodiscard]] int GetBy() const;
		[[nodiscard]] int GetCx() const;
		[[nodiscard]] int GetCy() const;
		[[nodiscard]] int GetDx() const;
		[[nodiscard]] int GetDy() const;

		void SetX(int x);
		void SetY(int y);
		void SetHeight(int h);
		void SetWidth(int w);
		void Reinitialize(int x, int y, int w, int h);

		[[nodiscard]] int GetHeight() const;
		[[nodiscard]] int GetWidth() const;
		bool operator==(const ABCDRectangle& other) const;

		static bool Intersects(const ABCDRectangle& a, const ABCDRectangle& b);

		[[nodiscard]] Coordinate<int> GetCenter() const;
	private:
		int x,y,w,h;
	};

}