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
		Event(const Event& other) = delete;
		Event(const Event&& other) = delete;
		Event& operator=(const Event& other) = delete;
		Event& operator=(const Event&& other) = delete;

		bool Processed = false;
		bool IsSingleton = false;
		std::string Origin;

		EventId Id;
		virtual std::string ToString();
		virtual ~Event();
	private:
		// Game Object Id counter
		static int lastEventId;
	};

	std::string operator+(const std::string& str, const EventId& id);	
}



