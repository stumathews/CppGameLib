#include "ControllerMoveEvent.h"

using namespace std;

namespace gamelib
{
	ControllerMoveEvent::ControllerMoveEvent(const gamelib::Direction dir, const KeyState keyState = KeyState::Pressed):
		Event(ControllerMoveEventId), keyState(keyState)
	{
		Direction = dir;
	}

	string ControllerMoveEvent::ToString()
	{
		return "position_change_event";
	}
}
