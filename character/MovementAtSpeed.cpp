#include "MovementAtSpeed.h"

#include <cmath>

#include "file/SettingsManager.h"

namespace gamelib
{
	MovementAtSpeed::MovementAtSpeed(const int speed, const Direction direction, const unsigned int deltaMs)
		: speed(speed), direction(direction), deltaMs(deltaMs)
	{
	}

	Direction MovementAtSpeed::GetDirection()
	{
		if (direction == Direction::Down)
		{
			if(speed < 0) { return Direction::Up; }
		}

		if (direction == Direction::Up)
		{
			if(speed < 0) { return Direction::Down; }
		}

		if (direction == Direction::Left)
		{
			if(speed < 0) { return Direction::Right; }
		}

		if (direction == Direction::Right)
		{
			if(speed < 0) { return Direction::Left; }
		}

		return direction;
	}

	unsigned short MovementAtSpeed::GetPixelsToMove()
	{
		return static_cast<unsigned short>(abs(speed));
	}

	Coordinate<int> MovementAtSpeed::GetPosition(const Coordinate<int> currentPosition)
	{
		Coordinate newPosition { currentPosition.GetX(), currentPosition.GetY()};

		if (direction == Direction::Down)
		{
			newPosition.SetY(currentPosition.GetY() + speed * 1);
		}

		if (direction == Direction::Up)
		{
			newPosition.SetY(currentPosition.GetY() - speed * 1);
		}

		if (direction == Direction::Left)
		{
			newPosition.SetX(currentPosition.GetX() - speed * 1);
		}

		if (direction == Direction::Right)
		{
			newPosition.SetX(currentPosition.GetX() + speed * 1);
		}
		return newPosition;
	}
}
