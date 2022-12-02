#pragma once
#include "pch.h"
#include <memory>
#include <string>
#include <Direction.h>
#include "IMovement.h"

class Room;

/// <summary>
/// A movement is defined as a movement in a specified direction
/// </summary>
class Movement : public gamelib::IMovement
{
public:

	Movement(gamelib::Direction inDirection, int pixelsToMove) : direction(inDirection), totalTargetMovePixels(pixelsToMove) { }
	virtual gamelib::Direction GetDirection() override { return direction; }
	virtual unsigned short GetPixelsToMove() override { return totalTargetMovePixels; }
private:	
	unsigned int totalTargetMovePixels;
	gamelib::Direction direction;

};

