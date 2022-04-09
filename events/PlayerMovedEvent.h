#pragma once
#include <memory>
#include "Event.h"
#include "ControllerMoveEvent.h"
namespace gamelib
{
	class PlayerMovedEvent final : public Event
	{
	public:
		explicit PlayerMovedEvent(Direction direction) :
			Event(EventType::PlayerMovedEventType), direction(direction)
		{
		}

		
		std::string ToString() override;
		const Direction direction;
	};
}

