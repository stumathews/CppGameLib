#pragma once
#include "Event.h"
#include "Direction.h"

namespace gamelib
{	
	class ControllerMoveEvent : public Event
	{
	public:
		explicit ControllerMoveEvent(Direction dir);
		Direction Direction;
		std::string ToString() override;
	};
}


