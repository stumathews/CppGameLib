#include "ControllerMoveEvent.h"

using namespace std;
namespace gamelib
{
	ControllerMoveEvent::ControllerMoveEvent(const gamelib::Direction dir): Event(EventType::ControllerMoveEvent)
	{
		Direction = dir;
	}

	string ControllerMoveEvent::ToString()
	{
		return "position_change_event";
	}
}
