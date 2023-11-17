#pragma once
#include <map>

#include "IMovement.h"
#include <events/ControllerMoveEvent.h>

namespace gamelib
{
	class StatefulMove :  public gamelib::IMovement
	{
	public:
		StatefulMove(int speed, const std::map<Direction, ControllerMoveEvent::KeyState>& movementAcceleration, unsigned long deltaMs);
		~StatefulMove() override = default;
		gamelib::Direction GetDirection() override;
		unsigned short GetPixelsToMove() override {return 0;}
		gamelib::Coordinate<int> GetPosition(gamelib::Coordinate<int> currentPosition) override;
		bool SupportsPositionalMovement() override { return true;}
		std::map<Direction, ControllerMoveEvent::KeyState> movementAcceleration;
		int speed;
		unsigned long deltaMs;
	};
}

