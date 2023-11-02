#pragma once
#include <objects/GameObject.h>

#include "DrawableText.h"
#include "geometry/Line.h"
#include "objects/DrawableGameObject.h"

namespace gamelib
{
	class DrawableFrameRate final : public DrawableText
	{
	public:
		explicit DrawableFrameRate(SDL_Rect* bounds);

		/// <summary>
		/// Update the frame rate
		/// </summary>
		/// <param name="deltaMs"></param>
		void Update(unsigned long deltaMs) override;
		void Draw(SDL_Renderer* renderer) override;

	private:
		unsigned long accumulatedUpdateTime = 0;
		unsigned int countUpdates = 0;
		unsigned int framesPerSecond = 0;
		SDL_Rect* drawBounds = {};

		// list of frame rates over time { ms, rate }
		std::vector<std::tuple<int, int>> sampleRates {};
		int accumulatedSampleWindowTime {};
		int MaxTotalSampleDurationMs = 30 * 1000;
		int sampleDurationInSecs = 1;
		int sampleDurationInMilliSecs = sampleDurationInSecs * 1000;

		int currentSampleTimeMs {};
	};
}