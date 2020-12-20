#include "rect_details.h"

namespace gamelib
{
		
	int rect_details::get_ax() const
	{
		return this->x;
	}

	int rect_details::get_ay() const
	{
		return this->y;
	}

	int rect_details::get_bx() const
	{
		return get_ax() + w;
	}

	int rect_details::get_by() const
	{
		return get_ay();
	}

	int rect_details::get_cx() const
	{
		return get_bx();
	}

	int rect_details::get_cy() const
	{
		return get_by() + w;
	}

	int rect_details::get_dx() const
	{
		return get_ax();
	}

	int rect_details::get_dy() const
	{
		return get_ay() + w;
	}

	void rect_details::set_x(int x)
	{
		this->x = x;
	}

	void rect_details::set_y(int y)
	{
		this->y = y;
	}

	void rect_details::set_h(int h)
	{
		this->h = h;
	}

	void rect_details::set_w(int w)
	{
		this->w = w;
	}

	void rect_details::init(int x, int y, int w, int h)
	{
		set_x(x);
		set_y(y);
		set_w(w);
		set_h(h);
	}

	coordinate<int> rect_details::get_a1(int intervalOfWOrH) const
	{
		return {get_ax() + this->w * intervalOfWOrH, get_ay()};
	}

	coordinate<int> rect_details::get_b1(int intervalOfWOrH) const
	{
		return {get_bx() - (this->w * intervalOfWOrH), get_by()};
	}

	coordinate<int> rect_details::get_c1(int intervalOfWOrH) const
	{
		return {get_cx() - (this->w * intervalOfWOrH), get_cy()};
	}

	coordinate<int> rect_details::get_d1(int intervalOfWOrH) const
	{
		return {get_dx() + (this->w * intervalOfWOrH), get_dy()};
	}

	coordinate<int> rect_details::get_a2(int intervalOfWOrH) const
	{
		return {get_ax(), get_ay() + (this->w * intervalOfWOrH)};
	}

	coordinate<int> rect_details::get_b2(int intervalOfWOrH) const
	{
		return {get_bx(), get_by() + (this->w * intervalOfWOrH)};
	}

	coordinate<int> rect_details::get_c2(int intervalOfWOrH) const
	{
		return {get_cx(), get_cy() - (this->w * intervalOfWOrH)};
	}

	coordinate<int> rect_details::get_d2(int intervalOfWOrH) const
	{
		return {get_dx(), get_dy() - this->w * intervalOfWOrH};
	}
}
