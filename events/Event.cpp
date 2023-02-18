#include "Event.h"
#include "../common/Common.h"


namespace gamelib
{
	std::string Event::ToString() { return Id.Name; }

	Event::~Event() = default;

	std::string operator+(const std::string& str, const EventId id) { return str + id.Name; }

	int Event::lastEventId = 0;
}
