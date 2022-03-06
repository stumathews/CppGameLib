#pragma once
#include "objects/GameObject.h"
namespace gamelib
{
	/// <summary>
	/// Object Can Draw on itself
	/// </summary>
	class DrawableGameObject : public gamelib::GameObject
	{
	public:
		DrawableGameObject(const bool is_visible, SettingsManager& settings_admin, EventManager& eventManager)
			: GameObject(is_visible, settings_admin, eventManager)
		{ }

		DrawableGameObject(const int x, const int y, const bool is_visible, SettingsManager& settings_admin, EventManager& eventAdmin)
			: GameObject(x, y, is_visible, settings_admin, eventAdmin)
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

