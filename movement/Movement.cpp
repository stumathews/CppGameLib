#include "pch.h"
#include "Movement.h"
#include <memory>
#include <common/Logger.h>
#include <sstream>
#include <util/SettingsManager.h>

int Movement::id = 0;

Movement::Movement(float durationMs, std::string movementTargetId, int maxPixelsToMove, bool debug)
{
	id++;

	// The expected total movement duration
	this->durationMs = durationMs;

	// The total number of pixels to have moved when the movement duration is completed
	this->totalTargetMovePixels = maxPixelsToMove;

	// Interval or length of distance to move in a millisecond
	this->pixelsPerMs = totalTargetMovePixels / durationMs;
	this->msPerPixel = durationMs / maxPixelsToMove;

	// Keep travel between update invervals how many pixels the movement has completed so far
	this->pixelsTraveled = 0;

	// Number of pixels to move (dynamicially updated)
	this->pixelsToMove = 0;

	// Indication if the number of total expected pixels moxed or duration is met
	this->isComplete = false;

	// 
	this->movementTargetId = movementTargetId;

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
		message << id << ": deltaMS:" << deltaMs << "Move " << pixelsToMove << " towards target " << movementTargetId << ". " << GetPixelsTraveled() << "/" << totalTargetMovePixels;
			
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

std::string Movement::GetMovementTargetId()
{
	return movementTargetId;
}
