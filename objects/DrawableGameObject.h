#pragma once
#include "objects/GameObject.h"
#include "graphic/GraphicAsset.h"
#include <SDL.h>
#include <memory>

namespace gamelib
{
	// Object Can Draw on itself in any way it wants to
	class DrawableGameObject : public GameObject
	{
	public:
		DrawableGameObject() = default;
		explicit DrawableGameObject(bool isVisible);
		DrawableGameObject(int x, int y, bool isVisible);
		DrawableGameObject(const std::string& name, const std::string& type, Coordinate<int> coordinate, bool isVisible);
		DrawableGameObject(const std::shared_ptr<GraphicAsset>& graphic, Coordinate<int> coordinate, bool isVisible);

		[[nodiscard]] bool HasColourKey() const;
		[[nodiscard]] bool HasGraphic() const;
		[[nodiscard]] SDL_Color GetColourKey() const;	
		[[nodiscard]] std::shared_ptr<GraphicAsset> GetGraphic() const;
		bool SupportsColourKey(bool yesNo);
		static void DrawFilledRect(SDL_Renderer* renderer, const SDL_Rect* dimensions, SDL_Color colour);
		void Draw(SDL_Renderer* renderer) override;
		void DrawGraphic(SDL_Renderer* renderer) const;
		void SetColourKey(const ColourKey& key);
		void SetColourKey(Uint8 r, Uint8 g, Uint8 b);
		void SetDefaults(const std::string& inName, const std::string& inType, bool isVisible, int x, int y);
		void SetGraphic(const std::shared_ptr<GraphicAsset>& graphicAsset);

		/// Each object may have a colour key enabled
		bool IsColorKeyEnabled {};

	private:
		void LogNoGraphicAssetProvidedError() const;

		// Game Object Graphic
		std::shared_ptr<GraphicAsset> graphic; // can be shared by other actors	

		// Game Object colour
		SDL_Color colourKey{};
	};
}

