#include "ABCDRectangle.h"

namespace gamelib
{
	ABCDRectangle::ABCDRectangle(const SDL_Rect rect) : ABCDRectangle(rect.x, rect.y, rect.w, rect.h) {}		
	ABCDRectangle::ABCDRectangle(const int x, const int y, const int w, const int h) : x(x), y(y), w(w), h(h) {}

	void ABCDRectangle::SetX(const int x) { this->x = x; }  // NOLINT(clang-diagnostic-shadow)
	void ABCDRectangle::SetY(const int y) { this->y = y; }  // NOLINT(clang-diagnostic-shadow)
	void ABCDRectangle::SetHeight(const int h) { this->h = h; }  // NOLINT(clang-diagnostic-shadow)
	void ABCDRectangle::SetWidth(const int w) { this->w = w; }  // NOLINT(clang-diagnostic-shadow)

	int ABCDRectangle::GetAx() const { return this->x; }
	int ABCDRectangle::GetAy() const { return this->y; }
	int ABCDRectangle::GetBx() const { return GetAx() + w; }
	int ABCDRectangle::GetBy() const { return GetAy(); }
	int ABCDRectangle::GetCx() const { return GetBx(); }
	int ABCDRectangle::GetCy() const { return GetBy() + h; }
	int ABCDRectangle::GetDx() const { return GetAx(); }
	int ABCDRectangle::GetDy() const { return GetAy() + h; }	
	int ABCDRectangle::GetWidth() const { return w; }
	int ABCDRectangle::GetHeight() const { return h; }

	bool ABCDRectangle::Intersects(const ABCDRectangle& a, const ABCDRectangle& b)
	{
		// Untested
		return a.GetAx() < b.GetCx() && 
			   a.GetCx() > a.GetAx() && 
			   a.GetAy() < b.GetCy() && 
			   a.GetCy() > b.GetAy();
	}

	void ABCDRectangle::Reinitialize(const int x, const int y, const int w, const int h) { SetX(x); SetY(y); SetWidth(w); SetHeight(h); }  // NOLINT(clang-diagnostic-shadow)

	Coordinate<int> ABCDRectangle::GetCenter() const
	{
		const int midX = GetAx() + GetWidth()/2;
		const int midY = GetAy() + GetHeight()/2;
		return {midX, midY};
	}
}
