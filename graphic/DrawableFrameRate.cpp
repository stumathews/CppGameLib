#include "DrawableFrameRate.h"
#include <graphic\RectDebugging.h>
#include <geometry/Line.h>
#include <SDL.h>

gamelib::DrawableFrameRate::DrawableFrameRate(SDL_Rect* bounds)
	: DrawableText(*bounds, "", {0,0,0,0})
{
	drawBounds = bounds;
}

void gamelib::DrawableFrameRate::Update(const unsigned long deltaMs) 
{	
	accumulatedUpdateTime += deltaMs;
	currentSampleTimeMs += deltaMs;
	accumulatedSampleWindowTime += deltaMs;

	if (accumulatedUpdateTime <= 1000)
	{
		accumulatedUpdateTime += deltaMs;
		countUpdates++;
	}
	else
	{
		// We got all the updates we can get within 1 sec
		framesPerSecond = countUpdates;
		accumulatedUpdateTime = countUpdates = 0; // reset
	}

	Text = std::to_string(framesPerSecond) + " FPS";

	if(currentSampleTimeMs >= sampleDurationInMilliSecs)
	{
		currentSampleTimeMs = 0;
		// collect a new sample
		sampleRates.emplace_back(accumulatedSampleWindowTime,framesPerSecond);
	}

	if(accumulatedSampleWindowTime > MaxTotalSampleDurationMs)
	{
		accumulatedSampleWindowTime = 0;
		sampleRates.clear();
	}
}

void gamelib::DrawableFrameRate::Draw(SDL_Renderer* renderer)
{
	DrawableText::Draw(renderer);
	const AbcdRectangle rect(*drawBounds);
	constexpr auto intervals = 4;
	constexpr auto ratio = 25;
	const auto pixelsPerInterval = rect.GetHeight() / (100/ratio);

	// draw a line every interval
	for(int i = 0; i < intervals;i++)
	{
		const auto jump = i * pixelsPerInterval;
		const Line marker(rect.GetDx(), rect.GetDy() - jump, rect.GetDx() + 5, rect.GetCy()-jump);
		const AbcdRectangle makerValueBounds(marker.X1 - 15, marker.Y1 - 5, 10, 10);
		DrawableText markerValue(makerValueBounds, std::to_string(i * ratio), Color);

		markerValue.Draw(renderer);
		SDL_RenderDrawLine(renderer, marker.X1, marker.Y1, marker.X2, marker.Y2);
	}

	for (auto& sample : sampleRates)
	{
		// draw framesPerSecond as a point somewhere on the widget
		std::stringstream format;
		
		const auto rate = std::get<1>(sample);
		const auto t = std::get<0>(sample);
		const auto totalSecsPassed = t / 1000;
		const auto xPerSecond = rect.GetDx() + (rect.GetWidth() / (MaxTotalSampleDurationMs / 1000)) * totalSecsPassed;

		const AbcdRectangle valueBounds(xPerSecond, rect.GetDy() + 25, rect.GetWidth()/2, rect.GetHeight()/2);
		//format << rate;
		//DrawableText value(valueBounds, format.str(), Color);

		const auto point = Line(xPerSecond,rect.GetDy() - rate, xPerSecond, rect.GetDy() - rate);

		SDL_RenderDrawLine(renderer, point.X1, point.Y1, point.X2, point.Y2);
		
	}
	




};


