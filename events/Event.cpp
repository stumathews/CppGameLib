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
		return "generic event";
	}

	std::string operator+(const std::string& str, const EventType type)
	{
		return str + std::to_string(as_integer(type));
	}

	/*template <typename T>
	std::shared_ptr<T> Event::DownCast()
	{
		return std::make_shared<T>(dynamic_cast<T>(this));
	}*/
}
