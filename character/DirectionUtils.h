#pragma once
#include <string>
#include "Direction.h"

namespace gamelib
{
class DirectionUtils
	{
	public:
				
		static const char* ToString(Direction direction);

		static Direction GetRandomDirection();

		static Direction GetOppositeDirectionTo(Direction direction);

		static Direction FromDirectionString(const std::string& direction);
	};
}
