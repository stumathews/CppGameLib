#pragma once
#include <string>
#include <sstream>
#include "EventId.h"

namespace gamelib
{
	class Event
	{
	public:
		explicit Event(EventId id): Id(std::move(id)) {}
		bool Processed = false;
		std::string Origin;
		EventId Id;
		virtual std::string ToString();
		virtual ~Event();
	private:
		// Game Object Id counter
		static int lastEventId;
	};

	std::string operator+(const std::string& str, EventId id);	
}



