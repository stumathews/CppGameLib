#include "pch.h"
#include "Hotspot.h"
#include <SDL.h>

namespace gamelib
{
	void Hotspot::Update(gamelib::coordinate<int> inPosition)
	{
		Position = inPosition;
	}

	gamelib::coordinate<int> Hotspot::CalculateHotspotPosition(int x, int y)
	{
		auto mid_x = x + ParentWidth / 2;
		auto mid_y = y + ParentWidth / 2;
		return gamelib::coordinate<int>(mid_x, mid_y);
	}

	gamelib::coordinate<int> Hotspot::GetPosition() { return CalculateHotspotPosition(Position.GetX(), Position.GetY()); }

	void Hotspot::Draw(SDL_Renderer* renderer)
	{
		auto hotspotPosition = GetPosition();
		SDL_Rect point_bounds =
		{
			hotspotPosition.GetX(),
			hotspotPosition.GetY(),
			(int) Width,
			(int) Width
		};
		DrawFilledRect(renderer, &point_bounds, { 255, 0 ,0 ,0 });		
	}
}