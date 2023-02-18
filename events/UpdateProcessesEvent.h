#pragma once
#include <events/Event.h>
namespace gamelib
{
	
const static EventId UpdateProcessesEventId(UpdateProcesses, "UpdateProcesses");
	class UpdateProcessesEvent : public Event
	{
	public:
		UpdateProcessesEvent(): Event(UpdateProcessesEventId) {}
		std::string ToString() override { return "UpdateProcessesEvent"; }
	};
}