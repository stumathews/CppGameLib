#pragma once
#include "Event.h"
#include "character/Direction.h"
#include "EventNumbers.h"

namespace gamelib
{
	const static EventId ControllerMoveEventId(ControllerMove, "ControllerMoveEvent");

	class ControllerMoveEvent final : public Event
	{		
	public:
		explicit ControllerMoveEvent(Direction dir);
		Direction Direction;
		std::string ToString() override;
	};
}


