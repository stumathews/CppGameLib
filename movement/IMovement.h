#pragma once
#include <Direction.h>

namespace gamelib
{
	class IMovement
	{
	public:
		virtual ~IMovement() = default;
		virtual Direction GetDirection() = 0;
		virtual unsigned short GetPixelsToMove() = 0;
	};
}