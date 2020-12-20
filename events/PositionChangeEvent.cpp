#include "PositionChangeEvent.h"

using namespace std;
namespace gamelib
{
	position_change_event::position_change_event(Direction dir): event(event_type::PositionChangeEventType)
	{
		direction = dir;
	}

	string position_change_event::to_str()
	{
		return "position_change_event";
	}
}
