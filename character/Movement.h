#pragma once
#include <character/Direction.h>
#include "IMovement.h"

class Room;

/// <summary>
/// A movement is defined as a movement in a specified direction
/// </summary>
class Movement final : public gamelib::IMovement
{
public:

	Movement(const gamelib::Direction inDirection, const int pixelsToMove = 1) : totalTargetMovePixels(pixelsToMove),
	                                                                         direction(inDirection) { }
	gamelib::Direction GetDirection() override { return direction; }
	unsigned short GetPixelsToMove() override { return static_cast<unsigned short>(totalTargetMovePixels); }
private:	
	unsigned int totalTargetMovePixels;
	gamelib::Direction direction;

};

