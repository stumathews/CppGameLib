#pragma once
#include "objects/GameObject.h"
#include <SDL.h>

namespace gamelib
{

	/// <summary>
	/// Object Can Draw on itself
	/// </summary>
	class DrawableGameObject : public gamelib::GameObject
	{
	public:
		DrawableGameObject(const bool is_visible)
			: GameObject(is_visible)
		{ }

		DrawableGameObject(const int x, const int y, const bool is_visible)
			: GameObject(x, y, is_visible)
		{ }

		/// <summary>
		/// Draw A filled rectangle
		/// </summary>
		/// <param name="renderer">Renderer</param>
		/// <param name="rectable dimensions"></param>
		/// <param name="colour">colour</param>
		void DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* dimensions, SDL_Color colour);
	};
}

