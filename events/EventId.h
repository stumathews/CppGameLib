#pragma once

#ifndef EVENTID_H
#define EVENTID_H

#include <string>

namespace gamelib 
{
    class EventId
    {
    public:
        EventId(const int pid, std::string name, const int sid = 0);
        bool operator== (const EventId& other) const;
        bool operator!=(const EventId& other) const;
        bool operator< (EventId const& other) const;
        const int PrimaryId;
        const int SecondaryId;
        const std::string Name;
    };
}

#endif
