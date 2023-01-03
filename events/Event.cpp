#include "Event.h"
#include "../common/Common.h"


namespace gamelib
{
	Event::Event(const EventType type): Type(type)
	{
		Id = lastEventId++;
	}

	EventType Event::GetGameObjectType() { return Type; }
	std::string Event::ToString() { return gamelib::ToString(Type); }

	Event::~Event() = default;

	std::string operator+(const std::string& str, const EventType type) { return str + std::to_string(ToInteger(type)); }

	int Event::lastEventId = 0;
}
