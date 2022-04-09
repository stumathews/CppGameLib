#include "ControllerMoveEvent.h"

using namespace std;
namespace gamelib
{
	ControllerMoveEvent::ControllerMoveEvent(Direction dir): Event(EventType::ControllerMoveEvent)
	{
		direction = dir;
	}

	string ControllerMoveEvent::ToString()
	{
		return "position_change_event";
	}
}
