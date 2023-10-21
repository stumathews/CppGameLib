#pragma once
#include "objects/GameObject.h"
#include "graphic/GraphicAsset.h"
#include <SDL.h>
#include <memory>

namespace gamelib
{

	/// <summary>
	/// Object Can Draw on itself in any way it wants to
	/// </summary>
	class DrawableGameObject : public GameObject
	{
	public:
		DrawableGameObject() = default;
		explicit DrawableGameObject(bool isVisible);
		DrawableGameObject(int x, int y, bool isVisible);
		DrawableGameObject(const std::string& name, const std::string& type, Coordinate<int> coordinate, bool isVisible);
		DrawableGameObject(const std::shared_ptr<GraphicAsset>& graphic, Coordinate<int> coordinate, bool isVisible);

		/// <summary>
		/// Draw Graphic
		/// </summary>
		/// <param name="renderer"></param>
		void DrawGraphic(SDL_Renderer* renderer) const;

		/// <summary>
		/// Has Graphic
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool HasGraphic() const;

		/// <summary>
		/// Set defaults
		/// </summary>
		/// <param name="inName"></param>
		/// <param name="inType"></param>
		/// <param name="isVisible"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		void SetDefaults(const std::string& inName, const std::string& inType, bool isVisible, int x, int y);

		/// <summary>
		/// Get Graphic
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] std::shared_ptr<GraphicAsset> GetGraphic() const;

		/// <summary>
		/// Draw A filled rectangle
		/// </summary>
		/// <param name="renderer">Renderer</param>
		/// <param name="dimensions">rectangular dimensions</param>
		/// <param name="colour">colour</param>
		static void DrawFilledRect(SDL_Renderer* renderer, const SDL_Rect* dimensions, SDL_Color colour);
		
		/// <summary>
		/// Set colour key
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		void SetColourKey(Uint8 r, Uint8 g, Uint8 b);
		void SetColourKey(const ColourKey& key);

		/// <summary>
		/// Set Graphic
		/// </summary>
		/// <param name="graphicAsset"></param>
		void SetGraphic(const std::shared_ptr<GraphicAsset>& graphicAsset);

		/// <summary>
		/// Draw
		/// </summary>
		/// <param name="renderer"></param>
		void Draw(SDL_Renderer* renderer) override;

		/// <summary>
		/// Get colour key
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] SDL_Color GetColourKey() const;
				
		/// <summary>
		/// Has Colour key
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool HasColourKey() const;

		/// <summary>
		/// If supports colour key
		/// </summary>
		/// <param name="yesNo"></param>
		/// <returns></returns>
		bool SupportsColourKey(bool yesNo);

		/// <summary>
		/// Each object may have a colour key enabled
		/// </summary>
		bool IsColorKeyEnabled {};

	private:
		void LogNoGraphicAssetProvidedError() const;
		// Game Object Graphic
		std::shared_ptr<GraphicAsset> graphic; // can be shared by other actors	

		// Game Object colour
		SDL_Color colourKey{};
	};
}

