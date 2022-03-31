#pragma once
#include "Event.h"

namespace gamelib
{
	enum class Direction {Up, Down, Left, Right};
	
	class PositionChangeEvent : public Event
	{
	public:
		explicit PositionChangeEvent(Direction dir);
		Direction direction;
		std::string ToString() override;
	};
}


