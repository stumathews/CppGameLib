#include "DrawableGameObject.h"
#include <SDL.h>
#include <geometry/Coordinate.h>
#include "graphic/GraphicAsset.h"
#include "file/Logger.h"

using namespace std;

namespace gamelib
{	
	DrawableGameObject::DrawableGameObject(const bool isVisible)
		: GameObject(isVisible)
	{
		SetDefaults("unknown", "unknown", IsVisible, 0, 0);
	}

	DrawableGameObject::DrawableGameObject(const int x, const int y, const bool isVisible)
		: GameObject(x, y, isVisible)
	{
		SetDefaults("unknown", "unknown", IsVisible, x, y);
	}


	DrawableGameObject::DrawableGameObject(const std::string& name, const std::string& type, const Coordinate<int>
	                                       coordinate, const bool isVisible)
		: GameObject(coordinate, isVisible)
	{
		SetDefaults(name, type, isVisible, coordinate.GetX(), coordinate.GetY());
	}

	DrawableGameObject::DrawableGameObject(const std::shared_ptr<GraphicAsset>& graphic, const Coordinate<int> coordinate,
	                                       const bool isVisible)
	{
		SetDefaults(graphic->Name, graphic->Type, isVisible, coordinate.GetX(), coordinate.GetY());
		SetGraphic(graphic);
	}

	void DrawableGameObject::SetDefaults(const std::string& inName, const std::string& inType, const bool isVisible, const int x, const int y)
	{
		IsVisible = isVisible;
		Position.SetX(x);
		Position.SetY(y);

		Bounds = {.x = Position.GetX(), .y = Position.GetY(), .w = 0 , .h = 0 };
		Name = inName;
		Type = inType;
		IsColorKeyEnabled = false;
		colourKey = {};
		SetGraphic(nullptr);
	}

	void DrawableGameObject::DrawFilledRect(SDL_Renderer* renderer, const SDL_Rect* dimensions, const SDL_Color colour)
	{
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, dimensions);
		SDL_RenderFillRect(renderer, dimensions);
	}

	void DrawableGameObject::LogNoGraphicAssetProvidedError() const
	{
		std::stringstream message;
		message << "No graphic associated with DrawableGameObject: " << Name;
		Logger::Get()->LogThis(message.str());
	}

	void DrawableGameObject::Draw(SDL_Renderer* renderer)
	{
		if (!IsVisible)
		{
			return;
		}

		if (HasGraphic())
		{
			DrawGraphic(renderer);
		}
		else
		{
			// This is OK, the descendant might be doing custom drawing
		}
	}

	void DrawableGameObject::SetColourKey(const Uint8 r, const Uint8 g, const Uint8 b)
	{
		colourKey.r = r;
		colourKey.g = g;
		colourKey.b = b;

		SupportsColourKey(true);
	}

	void DrawableGameObject::SetColourKey(const ColourKey& key)
	{
		colourKey.r = static_cast<Uint8>(key.Red);
		colourKey.g = static_cast<Uint8>(key.Green);
		colourKey.b = static_cast<Uint8>(key.Blue);
		
		SupportsColourKey(true);
	}

	SDL_Color DrawableGameObject::GetColourKey() const { return colourKey; }

	bool DrawableGameObject::HasColourKey() const { return IsColorKeyEnabled; }

	bool DrawableGameObject::SupportsColourKey(const bool yesNo)
	{
		IsColorKeyEnabled = yesNo;
		return IsColorKeyEnabled;
	}

	void DrawableGameObject::SetGraphic(const shared_ptr<GraphicAsset>& graphicAsset)
	{
		graphic = graphicAsset;

		// If we have a graphic that supports colour, lets always use it
		if( graphic != nullptr && graphic->HasColourKey())
		{
			SetColourKey(graphic->GetColourKey());
		}
	}
	
	void DrawableGameObject::DrawGraphic(SDL_Renderer* renderer) const
	{
		if (HasGraphic())
		{
			const auto graphicAsset = GetGraphic();

			if (graphicAsset->Type == "graphic")
			{
				// Draw graphic at the game object's current location
				const SDL_Rect drawLocation =
				{
					Position.GetX(), Position.GetY(),
					graphicAsset->GetViewPort().w,
					graphicAsset->GetViewPort().h
				};

				// Copy the texture (restricted by viewport) to the drawLocation on the screen
				SDL_RenderCopy(renderer, graphicAsset->GetTexture(), &graphicAsset->GetViewPort(), &drawLocation);
			}
			else
			{
				std::stringstream error;
				error << "cannot draw graphic asset. Unknown graphic asset type: " << graphicAsset->Type;
				Logger::Get()->LogThis(error.str());
			}
		}
		else
		{
			LogNoGraphicAssetProvidedError();
		}
	}
	
	std::shared_ptr<GraphicAsset> DrawableGameObject::GetGraphic() const
	{
		return graphic;
	}
	
	bool DrawableGameObject::HasGraphic() const
	{
		return graphic != nullptr;
	}
}
