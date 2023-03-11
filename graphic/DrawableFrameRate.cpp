#include "pch.h"
#include "DrawableFrameRate.h"
#include <util\RectDebugging.h>
#include <SDL.h>

gamelib::DrawableFrameRate::DrawableFrameRate(SDL_Rect* bounds) : DrawableText(*bounds, "", {0,0,0,0})
{
	drawBounds = bounds;
}

void gamelib::DrawableFrameRate::Update(const unsigned long deltaMs) 
{
	if (accumulatedUpdateTime <= 1000)
	{
		accumulatedUpdateTime += deltaMs;
		countUpdates++;
	}
	else
	{
		framesPerSecond = countUpdates;
		accumulatedUpdateTime = countUpdates = 0;
	}
	Text = std::to_string(framesPerSecond) + " FPS";
};


