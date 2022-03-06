#pragma once
#include "objects/GameObject.h"
namespace gamelib
{
	// Can Draw on itself
	class DrawingBase : public gamelib::GameObject
	{
	public:
		DrawingBase(const bool is_visible, SettingsManager& settings_admin, EventManager& event_admin)
			: GameObject(is_visible, settings_admin, event_admin)
		{
		}

		DrawingBase(const int x, const int y, const bool is_visible,
			SettingsManager& settings_admin, EventManager& eventAdmin)
			: GameObject(x, y, is_visible, settings_admin, eventAdmin)
		{
		}
		void DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* r, SDL_Color c);
	};
}

