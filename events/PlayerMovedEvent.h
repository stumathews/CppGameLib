#pragma once
#include "Event.h"
#include "EventNumbers.h"
#include "character/Direction.h"

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

