#include "DrawableFrameRate.h"
#include <graphic/RectDebugging.h>
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
	currentSampleTimeMs += static_cast<int>(deltaMs);
	accumulatedSampleWindowTime += static_cast<int>(deltaMs);

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

	if(accumulatedSampleWindowTime > maxTotalSampleDurationMs)
	{
		accumulatedSampleWindowTime = 0;
		sampleRates.clear();
	}
}

void gamelib::DrawableFrameRate::Draw(SDL_Renderer* renderer)
{
	//DrawableText::Draw(renderer);
	const AbcdRectangle rect(*drawBounds);
	constexpr auto intervals = 10;
	constexpr double ratio = 100.00 / intervals;
	const auto pixelsPerInterval = rect.GetHeight() / intervals;
	const auto pixelsPerValue = rect.GetHeight() / 100.00;

	// draw a line every interval
	for(int i = 0; i < intervals;i++)
	{
		const auto jump = i * pixelsPerInterval;
		const Line marker(rect.GetDx(), rect.GetDy() - jump, rect.GetDx() + 1, rect.GetCy()-jump);
		SDL_RenderDrawLine(renderer, marker.X1, marker.Y1, marker.X2, marker.Y2);
	}

	// draw line of rate from first sample to last (current) sample
	if (sampleRates.empty()) return;

	// sampleRates is a vector of tuples { time, rate }
	const auto firstSample = sampleRates[0];
	const auto lastSample = sampleRates[sampleRates.size() - 1];

	const auto firstRateValue = std::get<1>(firstSample);
	const auto lastRateValue = std::get<1>(lastSample);

	// We subtract because the y-axis is inverted and rect is not based 0,0 relative to rect
	const double y1 = rect.GetDy() - (pixelsPerValue * firstRateValue);
	const double y2 = rect.GetDy() - (pixelsPerValue * lastRateValue);

	const auto lastTime = std::get<0>(lastSample);
	const auto totalSecsPassed = lastTime / 1000;

	const int xAtLastTime = rect.GetDx() + (rect.GetWidth() / (maxTotalSampleDurationMs / 1000)) * totalSecsPassed;

	// Straight line from first to last sample
	const auto line = Line(rect.GetDx(), static_cast<int>(y1), xAtLastTime, static_cast<int>(y2));

	SDL_RenderDrawLine(renderer, line.X1, line.Y1, line.X2, line.Y2);
};