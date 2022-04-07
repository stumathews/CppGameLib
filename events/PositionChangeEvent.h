#pragma once
#include "Event.h"
#include "Direction.h"

namespace gamelib
{	
	class PositionChangeEvent : public Event
	{
	public:
		explicit PositionChangeEvent(Direction dir);
		Direction direction;
		std::string ToString() override;
	};
}


