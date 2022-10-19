#include "pch.h"
#include "Movement.h"
#include <memory>
#include <common/Logger.h>
#include <sstream>
#include <util/SettingsManager.h>
#include <Direction.h>

int Movement::id = 0;

Movement::Movement(float durationMs, gamelib::Direction direction, int maxPixelsToMove, bool debug)
{
	id++;

	this->direction = direction;

	// The expected total movement duration
	this->durationMs = durationMs;

	// The total number of pixels to have moved when the movement duration is completed
	totalTargetMovePixels = maxPixelsToMove;

	// Interval or length of distance to move in a millisecond
	pixelsPerMs = totalTargetMovePixels / durationMs;
	msPerPixel = durationMs / maxPixelsToMove;

	// Keep travel between update invervals how many pixels the movement has completed so far
	pixelsTraveled = 0;

	// Number of pixels to move (dynamicially updated)
	pixelsToMove = 0;

	// Indication if the number of total expected pixels moxed or duration is met
	isComplete = false;

	// Its possible to debug the movements internal operation
	this->debug = debug;
}

void Movement::Update(float deltaMs)
{	
	if(pixelsTraveled < totalTargetMovePixels)
	{
		pixelsToMove = ceil(pixelsPerMs * deltaMs);
	}
	else
	{
		isComplete = true;
	}

	if(debug)
	{
		std::stringstream message;
		message << "Movement #" << id << ": Delta Ms: " << deltaMs << " Move " << pixelsToMove << "pixels towards target " << gamelib::ToString(direction) << ". " << GetPixelsTraveled() << "/" << totalTargetMovePixels;
			
		gamelib::Logger::Get()->LogThis(message.str());
	}
}

bool Movement::IsComplete()
{
	return isComplete;
}

unsigned int Movement::TakePixelsToMove()
{	
	SpendPixels();
	return pixelsToMove;
}

unsigned int Movement::PreviewPixelsToMove()
{
	return pixelsToMove;
}

unsigned int Movement::GetPixelsTraveled()
{	
	return pixelsTraveled;
}

unsigned int Movement::GetPixelsStillToTravel()
{
	return totalTargetMovePixels - pixelsTraveled;
}

void Movement::SpendPixels()
{
	pixelsTraveled += pixelsToMove;
}
