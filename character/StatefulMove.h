#pragma once
#include <map>

#include "IMovement.h"
#include <events/ControllerMoveEvent.h>

namespace gamelib
{
	class StatefulMove :  public IMovement
	{
	public:
		StatefulMove(int speed, const std::map<Direction, ControllerMoveEvent::KeyState>& movementAcceleration, unsigned long deltaMs);
		~StatefulMove() override = default;
		Direction GetDirection() override;
		unsigned short GetPixelsToMove() override {return 0;}
		Coordinate<int> GetPosition(Coordinate<int> currentPosition) override;
		bool SupportsPositionalMovement() override { return true;}
		std::map<Direction, ControllerMoveEvent::KeyState> movementAcceleration;
		int speed;
		unsigned long deltaMs;
	};
}

