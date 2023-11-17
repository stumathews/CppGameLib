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
		enum class KeyState { Pressed, Released, Unknown };
		explicit ControllerMoveEvent(Direction dir, KeyState keyState);

		Direction Direction;
		
		std::string ToString() override;
		[[nodiscard]] KeyState GetKeyState() const { return keyState; }
		private:
		const KeyState keyState;
	};
}


