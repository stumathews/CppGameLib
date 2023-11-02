#pragma once
#include <character/Direction.h>
#include <geometry/Coordinate.h>

namespace gamelib
{
	class IMovement
	{
	public:
		virtual ~IMovement() = default;
		virtual Direction GetDirection() = 0;
		virtual unsigned short GetPixelsToMove() = 0;
		virtual Coordinate<int> GetPosition(Coordinate<int> currentPosition) = 0;
		virtual bool SupportsPositionalMovement() = 0;
	};
}