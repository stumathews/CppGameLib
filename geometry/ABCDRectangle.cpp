#include "ABCDRectangle.h"

namespace gamelib
{
	AbcdRectangle::AbcdRectangle(const SDL_Rect rect) : AbcdRectangle(rect.x, rect.y, rect.w, rect.h) {}		
	AbcdRectangle::AbcdRectangle(const int x, const int y, const int w, const int h) : x(x), y(y), w(w), h(h) {}

	void AbcdRectangle::SetX(const int x) { this->x = x; }  // NOLINT(clang-diagnostic-shadow)
	void AbcdRectangle::SetY(const int y) { this->y = y; }  // NOLINT(clang-diagnostic-shadow)
	void AbcdRectangle::SetHeight(const int h) { this->h = h; }  // NOLINT(clang-diagnostic-shadow)
	void AbcdRectangle::SetWidth(const int w) { this->w = w; }  // NOLINT(clang-diagnostic-shadow)

	int AbcdRectangle::GetAx() const { return this->x; }
	int AbcdRectangle::GetAy() const { return this->y; }
	int AbcdRectangle::GetBx() const { return GetAx() + w; }
	int AbcdRectangle::GetBy() const { return GetAy(); }
	int AbcdRectangle::GetCx() const { return GetBx(); }
	int AbcdRectangle::GetCy() const { return GetBy() + h; }
	int AbcdRectangle::GetDx() const { return GetAx(); }
	int AbcdRectangle::GetDy() const { return GetAy() + h; }	
	int AbcdRectangle::GetWidth() const { return w; }

	bool AbcdRectangle::operator==(const AbcdRectangle& other) const
	{
		return other.x == x && other.y == y && other.w == w && other.h == h;
	}

	int AbcdRectangle::GetHeight() const { return h; }

	bool AbcdRectangle::Intersects(const AbcdRectangle& a, const AbcdRectangle& b)
	{
		// Untested
		return a.GetAx() < b.GetCx() && 
			   a.GetCx() > a.GetAx() && 
			   a.GetAy() < b.GetCy() && 
			   a.GetCy() > b.GetAy();
	}

	void AbcdRectangle::Reinitialize(const int x, const int y, const int w, const int h) { SetX(x); SetY(y); SetWidth(w); SetHeight(h); }  // NOLINT(clang-diagnostic-shadow)

	Coordinate<int> AbcdRectangle::GetCenter() const
	{
		const int midX = GetAx() + GetWidth()/2;
		const int midY = GetAy() + GetHeight()/2;
		return {midX, midY};
	}
}
