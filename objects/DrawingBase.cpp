#include "DrawingBase.h"
namespace gamelib
{
	void DrawableGameObject::DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* rect, SDL_Color colour)
	{
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, rect);
		SDL_RenderFillRect(renderer, rect);
	}
}
