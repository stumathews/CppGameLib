#include "DrawingBase.h"
namespace gamelib
{
	void DrawingBase::DrawFilledRect(SDL_Renderer* renderer, SDL_Rect* r, SDL_Color c)
	{
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		SDL_RenderDrawRect(renderer, r);
		SDL_RenderFillRect(renderer, r);
	}
}
