#include "pch.h"
#include "Hotspot.h"
#include <SDL.h>

namespace gamelib
{
	SDL_Rect Hotspot::GetBounds()
	{
		auto hotspotPosition = GetPosition();
		return SDL_Rect { hotspotPosition.GetX(), hotspotPosition.GetY(), (int)Width, (int)Width };
	}

	gamelib::coordinate<int> Hotspot::CalculateHotspotPosition(int x, int y)
	{
		// calculate the center of the parent as the position of the top-left corner of the hotspot 
		auto mid_x = x + ParentWidth / 2;
		auto mid_y = y + ParentWidth / 2;

		// The adjust it so that the center of the hotspor
		mid_x -= Width / 2;
		mid_y -= Width / 2;
		return gamelib::coordinate<int>(mid_x, mid_y);
	}

	gamelib::coordinate<int> Hotspot::CalculateHotspotPosition() { return CalculateHotspotPosition(ParentPosition.GetX(), ParentPosition.GetY()); }
	gamelib::coordinate<int> Hotspot::GetPosition() { return CalculateHotspotPosition(Position.GetX(), Position.GetY()); }
	void Hotspot::Update(gamelib::coordinate<int> parentPosition) { Position = parentPosition; }

	void Hotspot::Draw(SDL_Renderer* renderer)
	{
		auto bounds = GetBounds();
		DrawFilledRect(renderer, &bounds, { 255, 0 ,0 ,0 });		
	}
}