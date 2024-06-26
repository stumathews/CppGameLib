#include "EventId.h"

namespace gamelib 
{
	EventId::EventId(const int pid, std::string name, const int sid) : PrimaryId(pid), SecondaryId(sid), Name(std::move(name))
	{}

	bool EventId::operator==(const EventId& other) const
	{
		return other.PrimaryId == PrimaryId && other.SecondaryId == SecondaryId;
	}

	bool EventId::operator!=(const EventId& other) const
	{
		return !(other == *this);
	}

	bool EventId::operator<(EventId const& other) const
	{
		return std::tie(PrimaryId, SecondaryId) < std::tie(other.PrimaryId, other.SecondaryId);
	}
}
