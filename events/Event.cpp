#include "Event.h"
#include "../common/Common.h"


namespace gamelib
{
	Event::Event(EventType type): type(type)
	{
		Id = Event::lastEventId++;
	}

	EventType Event::GetGameObjectType() { return type; }
	std::string Event::ToString() { return gamelib::ToString(type); }
	std::string operator+(const std::string& str, const EventType type) { return str + std::to_string(ToInteger(type)); }

	int Event::lastEventId = 0;
}
