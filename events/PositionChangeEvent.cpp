#include "PositionChangeEvent.h"

using namespace std;
namespace gamelib
{
	PositionChangeEvent::PositionChangeEvent(Direction dir): Event(EventType::PositionChangeEventType)
	{
		direction = dir;
	}

	string PositionChangeEvent::to_str()
	{
		return "position_change_event";
	}
}
