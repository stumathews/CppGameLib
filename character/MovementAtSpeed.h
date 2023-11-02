#pragma once
#include "character/IMovement.h"
#include "geometry/Coordinate.h"

namespace gamelib
{
	class MovementAtSpeed final : public IMovement
	{
	public:
		MovementAtSpeed(int speed, Direction direction, unsigned deltaMs);
		Direction GetDirection() override;
		unsigned short GetPixelsToMove() override;
		Coordinate<int> GetPosition(Coordinate<int> currentPosition) override;
		bool SupportsPositionalMovement() override { return true;}
		int speed;
		Direction direction;
		unsigned deltaMs;
	private:
		float movementStepPerFrame;
	};
}

