#pragma once
#include <objects/GameObject.h>

#include "DrawableText.h"
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

	private:
		unsigned long accumulatedUpdateTime = 0;
		unsigned int countUpdates = 0;
		unsigned int framesPerSecond = 0;
		SDL_Rect* drawBounds = {};
	};
}