#pragma once
#include <geometry/Coordinate.h>
#include <objects/DrawableGameObject.h>

namespace gamelib
{
	class Hotspot final : DrawableGameObject
	{
	public:
		Hotspot(const Coordinate<int> parentPosition, const unsigned int inParentWidth, const unsigned int inParentHeight, const unsigned int inWidth) : DrawableGameObject()
		{
			ParentPosition = parentPosition;
			ParentWidth = inParentWidth;
			ParentHeight = inParentHeight;
			Width = inWidth;
		}

		unsigned int ParentWidth;
		unsigned int ParentHeight;
		unsigned int Width;

		[[nodiscard]] Coordinate<int> GetPosition() const;
		Coordinate<int> ParentPosition;
		void Draw(SDL_Renderer* renderer) override;
		[[nodiscard]] Coordinate<int> CalculateHotspotPosition(int x, int y) const;
		[[nodiscard]] Coordinate<int> CalculateHotspotPosition() const;
		void Update(Coordinate<int> parentPosition);
		void Update(const unsigned long deltaMs) override {}
		GameObjectType GetGameObjectType() override { return GameObjectType::hotspot; }
		[[nodiscard]] SDL_Rect GetBounds() const;
	};
}