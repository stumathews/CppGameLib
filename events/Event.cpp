#include "Event.h"
#include "../common/Common.h"


namespace gamelib
{
	Event::Event(EventType type, int event_id) : type(type), eventId(event_id)
	{
		// using member init only
	}

	EventType Event::GetGameObjectType()
	{
		return type;
	}

	std::string Event::ToString()
	{
		return gamelib::ToString(type);
	}

	std::string operator+(const std::string& str, const EventType type)
	{
		return str + std::to_string(ToInteger(type));
	}
}
