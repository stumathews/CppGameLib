#include "StatefulMove.h"

namespace gamelib
{
	Coordinate<int> StatefulMove::GetPosition(const Coordinate<int> currentPosition)
	{
		Coordinate newPosition { currentPosition.GetX(), currentPosition.GetY()};

		auto move = [&](const Direction dir)
		{
			if(!movementAcceleration.contains(dir)) return 0;
			return 1 * speed * (movementAcceleration[dir] == ControllerMoveEvent::KeyState::Pressed ? 1 : 0);
		};

		const auto moveRight = move(Direction::Right);
		const auto moveLeft = move(Direction::Left);
		const auto moveUp = move(Direction::Up);
		const auto moveDown = move(Direction::Down);

		if(moveRight)
		{
			newPosition.SetX(currentPosition.GetX() + moveRight);
		}
		else
		{
			newPosition.SetX(currentPosition.GetX() - moveLeft);
		}

		if(moveUp)
		{
			newPosition.SetY(currentPosition.GetY() - moveUp);
		}
		else
		{
			newPosition.SetY(currentPosition.GetY() + moveDown);	
		}
		
		return newPosition;
	}

	StatefulMove::StatefulMove(const int speed, 
		const std::map<Direction, ControllerMoveEvent::KeyState>& movementAcceleration,  // NOLINT(modernize-pass-by-value)
		const unsigned long deltaMs): movementAcceleration(movementAcceleration), speed(speed), deltaMs(deltaMs)
	{
	}

	Direction StatefulMove::GetDirection()
	{
		if (movementAcceleration[Direction::Up] == ControllerMoveEvent::KeyState::Pressed) return Direction::Up;
		if (movementAcceleration[Direction::Down] == ControllerMoveEvent::KeyState::Pressed) return Direction::Down;
		if (movementAcceleration[Direction::Left] == ControllerMoveEvent::KeyState::Pressed) return Direction::Left;
		if (movementAcceleration[Direction::Right] == ControllerMoveEvent::KeyState::Pressed) return Direction::Right;
		return Direction::None;
	}
}
