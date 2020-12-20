#pragma once
#include <SDL.h>
#include "util/Tuple.h"

namespace gamelib
{
	class rect_details
	{
	public:
		rect_details(const SDL_Rect rect) : rect_details(rect.x, rect.y, rect.w, rect.h){}
		rect_details(int x, int y, int w, int h) : x(x), y(y),w(w),h(h){}
		int get_ax() const;
		int get_ay() const;
		int get_bx() const;
		int get_by() const;
		int get_cx() const;
		int get_cy() const;
		int get_dx() const;
		int get_dy() const;

		void set_x(int x);
		void set_y(int y);
		void set_h(int h);
		void set_w(int w);

		void init(int x, int y, int w, int h);

		coordinate<int> get_a1(int intervalOfWOrH) const;

		coordinate<int> get_b1(int intervalOfWOrH) const;

		coordinate<int> get_c1(int intervalOfWOrH) const;

		coordinate<int> get_d1(int intervalOfWOrH) const;

		coordinate<int> get_a2(int intervalOfWOrH) const;

		coordinate<int> get_b2(int intervalOfWOrH) const;

		coordinate<int> get_c2(int intervalOfWOrH) const;

		coordinate<int> get_d2(int intervalOfWOrH) const;

	private:
		int x,y,w,h;

	};

}