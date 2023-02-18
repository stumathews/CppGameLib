#pragma once
#include <string>

class EventId
{
public:
     EventId(const int id, std::string name);
	 bool operator== (const EventId& other) const;
     bool operator!=(const EventId& other) const;
     bool operator< (EventId const &  other) const;
     const int Id;
     const std::string Name;
 };
