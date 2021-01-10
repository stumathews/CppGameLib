#pragma once
#include "objects/GameObject.h"
namespace gamelib
{
	class DrawingBase : public gamelib::GameObject
	{
	public:
		DrawingBase(const bool is_visible, const std::shared_ptr<settings_manager>& settings_admin)
			: GameObject(is_visible, settings_admin)
		{
		}

		DrawingBase(const int x, const int y, const bool is_visible,
			const std::shared_ptr<settings_manager>& settings_admin)
			: GameObject(x, y, is_visible, settings_admin)
		{
		}
		void DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* r, SDL_Color c);
	};
}

