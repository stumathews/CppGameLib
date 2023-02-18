#include "EventId.h"

EventId::EventId(const int id, std::string name): Id(id), Name(std::move(name))
{}

bool EventId::operator==(const EventId& other) const
{
	return other.Id == Id && other.Name == Name;
}

bool EventId::operator!=(const EventId& other) const
{
	return !(other == *this);
}

bool EventId::operator<(EventId const & other) const
{
	return std::tie(Id, Name) < std::tie(other.Id, other.Name);
}
