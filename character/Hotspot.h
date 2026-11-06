#pragma once
#include <geometry/Coordinate.h>
#include <objects/DrawableGameObject.h>

namespace gamelib
{
	class Hotspot final : DrawableGameObject
	{
	public:
		Hotspot(const Coordinate<float> parentPosition, const unsigned int inParentWidth, const unsigned int inParentHeight, const unsigned int inWidth) : DrawableGameObject()
		{
			ParentPosition = parentPosition;
			ParentWidth = inParentWidth;
			ParentHeight = inParentHeight;
			Width = inWidth;
		}

		unsigned int ParentWidth;
		unsigned int ParentHeight;
		unsigned int Width;

		[[nodiscard]] Coordinate<float> GetPosition() const;
		Coordinate<float> ParentPosition;
		void Draw(SDL_Renderer* renderer) override;
		[[nodiscard]] Coordinate<float> CalculateHotspotPosition(int x, int y) const;
		[[nodiscard]] Coordinate<float> CalculateHotspotPosition() const;
		void Update(Coordinate<float> parentPosition);
		void Update(const gamelib::DeltaTime* deltaTime) override {}
		GameObjectType GetGameObjectType() override { return GameObjectType::hotspot; }
		[[nodiscard]] SDL_Rect GetBounds() const;
	};
}