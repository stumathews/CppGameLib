#pragma once
#include "Event.h"
#include "ControllerMoveEvent.h"

namespace gamelib
{
	class GameObject;
	const static EventId PlayerMovedEventTypeEventId(PlayerMovedEventType, "PlayerMovedEventType");
	class PlayerMovedEvent final : public Event
	{
	public:
		explicit PlayerMovedEvent(const Direction direction) :
			Event(PlayerMovedEventTypeEventId), Direction(direction)
		{
		}

		std::string ToString() override;
		const Direction Direction;
	};
}

