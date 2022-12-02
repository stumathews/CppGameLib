#pragma once
#include <util/Tuple.h>
#include <objects/DrawableGameObject.h>

namespace gamelib
{
	class Hotspot : gamelib::DrawableGameObject
	{
	public:
		Hotspot( coordinate<int> parentPosition, const unsigned int inParentWidth, const unsigned int inParentHeight, const unsigned int inWidth) : DrawableGameObject()
		{
			ParentPosition = parentPosition;
			ParentWidth = inParentWidth;
			ParentHeight = inParentHeight;
			Width = inWidth;
		}

		unsigned int ParentWidth;
		unsigned int ParentHeight;
		unsigned int Width;

		gamelib::coordinate<int> GetPosition();
		gamelib::coordinate<int> ParentPosition;
		virtual void Draw(SDL_Renderer* renderer) override;
		gamelib::coordinate<int> CalculateHotspotPosition(int x, int y);
		gamelib::coordinate<int> CalculateHotspotPosition();
		void Update(gamelib::coordinate<int> parentPosition);
		virtual void Update(float deltaMs) override {}
		virtual GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::Hotspot; }
		SDL_Rect GetBounds();
	};
}