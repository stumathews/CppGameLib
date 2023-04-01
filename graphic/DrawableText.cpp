#include "DrawableText.h"

#include "util/RectDebugging.h"

gamelib::DrawableText::DrawableText(SDL_Rect bounds, std::string text, const SDL_Color color = {0,0,0, 0}): Text(std::move(text)), Color(color)
{
	DrawBounds = bounds;
}

void gamelib::DrawableText::Update(unsigned long deltaMs)
{
}


void gamelib::DrawableText::Draw(SDL_Renderer* renderer) 
{
	RectDebugging::PrintInRect(renderer, Text, &DrawBounds, Color);	
}