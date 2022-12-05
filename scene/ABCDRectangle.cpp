#include "ABCDRectangle.h"

namespace gamelib
{
	ABCDRectangle::ABCDRectangle(const SDL_Rect rect) : ABCDRectangle(rect.x, rect.y, rect.w, rect.h) {}		
	ABCDRectangle::ABCDRectangle(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}

	void ABCDRectangle::SetX(int x) { this->x = x; }
	void ABCDRectangle::SetY(int y) { this->y = y; }
	void ABCDRectangle::SetHeight(int h) { this->h = h; }
	void ABCDRectangle::SetWidth(int w) { this->w = w; }

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

	void ABCDRectangle::reinitialize(int x, int y, int w, int h) { SetX(x); SetY(y); SetWidth(w); SetHeight(h); }

	coordinate<int> ABCDRectangle::GetCenter()
	{
		int mid_x = GetAx() + GetWidth()/2;
		int mid_y = GetAy() + GetHeight()/2;
		return coordinate<int>(mid_x, mid_y);
	}
}
