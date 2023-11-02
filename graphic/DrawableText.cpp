#include "DrawableText.h"

#include "graphic/RectDebugging.h"

gamelib::DrawableText::DrawableText(SDL_Rect bounds, std::string text, const SDL_Color color = {0,0,0, 0})
: DrawBounds(bounds), Text(std::move(text)), Color(color)
{
}

gamelib::DrawableText::DrawableText(const AbcdRectangle& bounds, std::string text, SDL_Color color)
: DrawBounds(SDL_Rect { bounds.GetAx(),bounds.GetAy(), bounds.GetWidth(), bounds.GetHeight()}), Text(std::move(text)), Color(color)
{
}

void gamelib::DrawableText::Update(unsigned long deltaMs)
{
}


void gamelib::DrawableText::Draw(SDL_Renderer* renderer) 
{
	RectDebugging::PrintInRect(renderer, Text, &DrawBounds, Color);	
}
