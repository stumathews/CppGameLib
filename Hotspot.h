#pragma once
#include <util/Tuple.h>
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
		void Update(float deltaMs) override {}
		GameObjectType GetGameObjectType() override { return GameObjectType::Hotspot; }
		[[nodiscard]] SDL_Rect GetBounds() const;
	};
}