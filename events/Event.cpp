#include "Event.h"
#include "../common/Common.h"

namespace gamelib
{
	Event::Event(EventType type, int event_id): type(type), event_id(event_id)
	{
		// using member init only
	}

	EventType Event::GetGameObjectType()
	{
		return type;
	}

	std::string Event::to_str()
	{
		return "generic event";
	}

	std::string operator+(const std::string& str, const EventType type)
	{
		return str + std::to_string(as_integer(type));
	}
}
