#pragma once
#include "Event.h"
#include "ControllerMoveEvent.h"
namespace gamelib
{
	class GameObject;
	class PlayerMovedEvent final : public Event
	{
	public:
		explicit PlayerMovedEvent(const Direction direction) :
			Event(EventType::PlayerMovedEventType), Direction(direction)
		{
		}

		std::string ToString() override;
		const Direction Direction;
	};
}

