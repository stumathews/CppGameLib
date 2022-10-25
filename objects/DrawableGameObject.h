#pragma once
#include "objects/GameObject.h"
#include "graphic/GraphicAsset.h"
#include <SDL.h>
#include <memory>

namespace gamelib
{

	/// <summary>
	/// Object Can Draw on itself
	/// </summary>
	class DrawableGameObject : public gamelib::GameObject
	{
	public:
		DrawableGameObject() = default;
		DrawableGameObject(const bool is_visible);

		DrawableGameObject(const int x, const int y, const bool is_visible);
		DrawableGameObject(gamelib::coordinate<int> coordinate, const bool is_visible);

		void DrawGraphic(SDL_Renderer* renderer) const;

		bool HasGraphic() const;

		void SetDefaults(bool isVisible, int x, int y);

		std::shared_ptr<GraphicAsset> GetGraphic() const;

		/// <summary>
		/// Draw A filled rectangle
		/// </summary>
		/// <param name="renderer">Renderer</param>
		/// <param name="dimensions">rectangular dimensions</param>
		/// <param name="colour">colour</param>
		void DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* dimensions, SDL_Color colour);

		void SetColourKey(const Uint8 r, const Uint8 g, const Uint8 b);

		void SetGraphic(std::shared_ptr<GraphicAsset> graphic);

		void Draw(SDL_Renderer* renderer) override;

		SDL_Color GetColourKey();
				
		bool HasColourKey();

		bool SupportsColourKey(bool yesNo);

	private:

		// Game Object Graphic
		std::shared_ptr<GraphicAsset> graphic; // can be shared by other actors

		/// <summary>
		/// Each object may have a colour key enabled
		/// </summary>
		bool isColorKeyEnabled;

		// Game Object colour
		SDL_Color colourKey;;
	};
}

