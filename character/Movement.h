#pragma once
#include <character/Direction.h>
#include "IMovement.h"
#include "exceptions/EngineException.h"

class Room;

namespace gamelib
{
	/// <summary>
	/// A movement is defined as a movement in a specified direction
	/// </summary>
	class Movement final : public IMovement
	{
	public:

		Movement(const Direction inDirection, const int pixelsToMove = 1) : totalTargetMovePixels(pixelsToMove),
		                                                                    direction(inDirection) { }

		Direction GetDirection() override { return direction; }
		unsigned short GetPixelsToMove() override { return static_cast<unsigned short>(totalTargetMovePixels); }
		Coordinate<int> GetPosition(Coordinate<int> currentPosition) override { THROW(2, "GetPosition Not Supported in Movement class", "Movement");}
		bool SupportsPositionalMovement() override { return false; }

	private:	
		unsigned int totalTargetMovePixels;
		Direction direction;

	};
}


