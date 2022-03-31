#include "PositionChangeEvent.h"

using namespace std;
namespace gamelib
{
	PositionChangeEvent::PositionChangeEvent(Direction dir): Event(EventType::PositionChangeEventType)
	{
		direction = dir;
	}

	string PositionChangeEvent::ToString()
	{
		return "position_change_event";
	}
}
