#include "DrawableGameObject.h"
#include <SDL.h>
#include <util/Tuple.h>
using namespace std;

namespace gamelib
{
	void DrawableGameObject::DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* region, SDL_Color colour)
	{
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, region);
		SDL_RenderFillRect(renderer, region);
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

		// We can draw the graphic resource if we have one however 
		// (this requires the decendents to call ensure they call GameObject::Draw() explicitly)
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

		SupportsColourKey(true);
	}

	/// <summary>
	/// Set defaults
	/// </summary>
	void DrawableGameObject::SetDefaults(bool isVisible, int x, int y)
	{
		this->isVisible = isVisible;
		/*this->x = x;
		this->y = y;*/
		Position.SetX(x);
		Position.SetY(y);

		// In this sensible?
		Bounds = { Position.GetX(), Position.GetY(), 0 , 0 };

		// Should all game Objects support ColourKey
		isColorKeyEnabled = false;
		colourKey = {};

		graphic = nullptr;
	}

	SDL_Color DrawableGameObject::GetColourKey()
	{
		return colourKey;
	}

	/// <summary>
	/// Determines if the colour key is enabled for this drawable object
	/// </summary>
	/// <returns></returns>
	bool DrawableGameObject::HasColourKey()
	{
		return isColorKeyEnabled;;
	}

	bool DrawableGameObject::SupportsColourKey(bool yesNo)
	{
		isColorKeyEnabled = yesNo;
		return isColorKeyEnabled;
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
					Position.GetX(), Position.GetY(),
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


	DrawableGameObject::DrawableGameObject(gamelib::coordinate<int> coordinate, const bool is_visible)
		: GameObject(coordinate, is_visible)
	{

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
