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
		DrawableGameObject(std::string name, std::string type, gamelib::coordinate<int> coordinate, const bool is_visible);

		/// <summary>
		/// Draw Graphic
		/// </summary>
		/// <param name="renderer"></param>
		void DrawGraphic(SDL_Renderer* renderer) const;

		/// <summary>
		/// Has Graphic
		/// </summary>
		/// <returns></returns>
		bool HasGraphic() const;

		/// <summary>
		/// Set defaults
		/// </summary>
		/// <param name="isVisible"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		void SetDefaults(std::string inName, std::string inType, bool IsVisible, int x, int y);

		/// <summary>
		/// Get Graphic
		/// </summary>
		/// <returns></returns>
		std::shared_ptr<GraphicAsset> GetGraphic() const;

		/// <summary>
		/// Draw A filled rectangle
		/// </summary>
		/// <param name="renderer">Renderer</param>
		/// <param name="dimensions">rectangular dimensions</param>
		/// <param name="colour">colour</param>
		void DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* dimensions, SDL_Color colour);

		/// <summary>
		/// Set colour key
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		void SetColourKey(const Uint8 r, const Uint8 g, const Uint8 b);

		/// <summary>
		/// Set Graphic
		/// </summary>
		/// <param name="graphic"></param>
		void SetGraphic(std::shared_ptr<GraphicAsset> Graphic);

		/// <summary>
		/// Draw
		/// </summary>
		/// <param name="renderer"></param>
		void Draw(SDL_Renderer* renderer) override;

		/// <summary>
		/// Get colour key
		/// </summary>
		/// <returns></returns>
		SDL_Color GetColourKey();
				
		/// <summary>
		/// Has COlour key
		/// </summary>
		/// <returns></returns>
		bool HasColourKey();

		/// <summary>
		/// If supports colour key
		/// </summary>
		/// <param name="yesNo"></param>
		/// <returns></returns>
		bool SupportsColourKey(bool yesNo);

	private:

		// Game Object Graphic
		std::shared_ptr<GraphicAsset> Graphic; // can be shared by other actors

		/// <summary>
		/// Each object may have a colour key enabled
		/// </summary>
		bool IsColorKeyEnabled;

		// Game Object colour
		SDL_Color ColourKey;
	};
}

