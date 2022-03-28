#include "DrawableGameObject.h"
#include <SDL.h>

using namespace std;

namespace gamelib
{
	void DrawableGameObject::DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* rect, SDL_Color colour)
	{
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, rect);
		SDL_RenderFillRect(renderer, rect);
	}

	/// <summary>
	/// Draw Game Object
	/// </summary>
	/// <param name="renderer"></param>
	void DrawableGameObject::Draw(SDL_Renderer* renderer)
	{
		// Game Object usually does not draw itself... but decendents do.

		if (!isVisible)
		{
			return;
		}

		// We can draw the graphic resource if we have one however (this requires the decendents to call ensure they call GameObject::Draw() explicitly)
		if (HasGraphic())
		{
			DrawGraphic(renderer);
		}
	}

	/// <summary>
	/// Set color key
	/// </summary>
	void DrawableGameObject::SetColourKey(const Uint8 r, const Uint8 g, const Uint8 b)
	{
		colourKey.r = r;
		colourKey.g = g;
		colourKey.b = b;
	}

	/// <summary>
	/// Set defaults
	/// </summary>
	void DrawableGameObject::SetDefaults(bool isVisible, int x, int y)
	{
		this->isVisible = isVisible;
		this->x = x;
		this->y = y;

		// In this sensible?
		bounds = { x, y, 0 , 0 };

		// Should all game Objects support ColourKey
		isColorKeyEnabled = false;

		red = 0x00;
		blue = 0xFF;
		green = 0x00;
	}


	/// <summary>
	/// Set the graphic
	/// </summary>
	/// <param name="graphic"></param>
	void DrawableGameObject::SetGraphic(shared_ptr<GraphicAsset> graphic)
	{
		this->graphic = graphic;
	}

	/// <summary>
	/// Draw graphic resource is one exists
	/// </summary>
	/// <param name="renderer"></param>
	void DrawableGameObject::DrawGraphic(SDL_Renderer* renderer) const
	{
		if (HasGraphic())
		{
			const auto graphic = GetGraphic();

			if (graphic->type == "graphic")
			{
				// Draw graphic at the game object's current location
				SDL_Rect drawLocation =
				{
					x, y,
					graphic->GetViewPort().w,
					graphic->GetViewPort().h
				};

				// Copy the texture (restricted by viewport) to the drawLocation on the screen
				SDL_RenderCopy(renderer, graphic->GetTexture(), &graphic->GetViewPort(), &drawLocation);
			}
			else
			{
				// Log error here
			}
		}
	}

	/// <summary>
	/// Get Graphic resource
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<GraphicAsset> DrawableGameObject::GetGraphic() const
	{
		return graphic;
	}

	DrawableGameObject::DrawableGameObject(const bool is_visible)
		: GameObject(is_visible)
	{
		SetDefaults(isVisible, 0, 0);
	}

	DrawableGameObject::DrawableGameObject(const int x, const int y, const bool is_visible)
		: GameObject(x, y, is_visible)
	{
		SetDefaults(isVisible, x, y);
	}

	/// <summary>
	/// Check if game object has graphic resource
	/// </summary>
	/// <returns></returns>
	bool DrawableGameObject::HasGraphic() const
	{
		return graphic != nullptr;
	}
}
