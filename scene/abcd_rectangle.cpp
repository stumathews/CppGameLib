#include "abcd_rectangle.h"

namespace gamelib
{
		
	int abcd_rectangle::get_ax() const
	{
		return this->x;
	}

	int abcd_rectangle::get_ay() const
	{
		return this->y;
	}

	int abcd_rectangle::get_bx() const
	{
		return get_ax() + w;
	}

	int abcd_rectangle::get_by() const
	{
		return get_ay();
	}

	int abcd_rectangle::get_cx() const
	{
		return get_bx();
	}

	int abcd_rectangle::get_cy() const
	{
		return get_by() + h;
	}

	int abcd_rectangle::get_dx() const
	{
		return get_ax();
	}

	int abcd_rectangle::get_dy() const
	{
		return get_ay() + h;
	}

	void abcd_rectangle::set_x(int x)
	{
		this->x = x;
	}

	void abcd_rectangle::set_y(int y)
	{
		this->y = y;
	}

	void abcd_rectangle::set_h(int h)
	{
		this->h = h;
	}

	void abcd_rectangle::set_w(int w)
	{
		this->w = w;
	}

	void abcd_rectangle::reinitialize(int x, int y, int w, int h)
	{
		set_x(x);
		set_y(y);
		set_w(w);
		set_h(h);
	}

	/*coordinate<int> abcd_rectangle::get_a1(int intervalOfWOrH) const
	{
		return {get_ax() + this->w * intervalOfWOrH, get_ay()};
	}

	coordinate<int> abcd_rectangle::get_a2(int intervalOfWOrH) const
	{
		return {get_ax(), get_ay() + (this->w * intervalOfWOrH)};
	}

	coordinate<int> abcd_rectangle::get_b1(int intervalOfWOrH) const
	{
		return {get_bx() - (this->w * intervalOfWOrH), get_by()};
	}

	coordinate<int> abcd_rectangle::get_c1(int intervalOfWOrH) const
	{
		return {get_cx() - (this->w * intervalOfWOrH), get_cy()};
	}

	coordinate<int> abcd_rectangle::get_d1(int intervalOfWOrH) const
	{
		return {get_dx() + (this->w * intervalOfWOrH), get_dy()};
	}

	

	coordinate<int> abcd_rectangle::get_b2(int intervalOfWOrH) const
	{
		return {get_bx(), get_by() + (this->w * intervalOfWOrH)};
	}

	coordinate<int> abcd_rectangle::get_c2(int intervalOfWOrH) const
	{
		return {get_cx(), get_cy() - (this->w * intervalOfWOrH)};
	}

	coordinate<int> abcd_rectangle::get_d2(int intervalOfWOrH) const
	{
		return {get_dx(), get_dy() - this->w * intervalOfWOrH};
	}*/
}
