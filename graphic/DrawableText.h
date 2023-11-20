#pragma once
#include "objects/DrawableGameObject.h"
#include "objects/GameObject.h"

namespace gamelib
{
	class DrawableText : public DrawableGameObject
	{
	public:
		explicit DrawableText(SDL_Rect bounds, std::string text, SDL_Color color);
		explicit DrawableText(const AbcdRectangle& bounds, std::string text, SDL_Color color);

		/// <summary>
		/// Update the frame rate
		/// </summary>
		/// <param name="deltaMs"></param>
		void Update(unsigned long deltaMs) override;

		/// <summary>
		/// All game objects can be drawn uniformly
		/// </summary>
		/// <param name="renderer"></param>
		void Draw(SDL_Renderer* renderer) override;

		/// <summary>
		/// Every game Object needs to identify what type of game object it is
		/// </summary>
		/// <returns></returns>
		GameObjectType GetGameObjectType() override { return GameObjectType::GameDefined; }
		
		SDL_Rect DrawBounds;
		std::string Text {};	
		SDL_Color Color {};
	};
}
