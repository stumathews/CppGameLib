#include "Hotspot.h"
#include <SDL.h>

namespace gamelib
{
	SDL_Rect Hotspot::GetBounds() const
	{
		const auto hotspotPosition = GetPosition();
		return SDL_Rect { static_cast<int>(hotspotPosition.GetX()), static_cast<int>(hotspotPosition.GetY()), static_cast<int>(Width), static_cast<int>(Width) };
	}

	Coordinate<float> Hotspot::CalculateHotspotPosition(const int x, const int y) const
	{
		// calculate the center of the parent as the position of the top-left corner of the hotspot 
		auto midX = x + ParentWidth / 2.0f;
		auto midY = y + ParentWidth / 2.0f;

		// The adjust it so that the center of the hotspot
		midX -= Width / 2;
		midY -= Width / 2;
		return {midX, midY};
	}

	Coordinate<float> Hotspot::CalculateHotspotPosition() const { return CalculateHotspotPosition(ParentPosition.GetX(), ParentPosition.GetY()); }
	Coordinate<float> Hotspot::GetPosition() const { return CalculateHotspotPosition(Position.GetX(), Position.GetY()); }
	void Hotspot::Update(const Coordinate<float> parentPosition) { Position = parentPosition; }

	void Hotspot::Draw(SDL_Renderer* renderer)
	{
		const auto bounds = GetBounds();
		DrawFilledRect(renderer, &bounds, { 255, 0 ,0 ,0 });		
	}
}