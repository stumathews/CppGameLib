#pragma once
#include <events/Event.h>
namespace gamelib
{
	class UpdateProcessesEvent : public Event
	{
	public:
		UpdateProcessesEvent(): Event(EventType::UpdateProcesses) {}
		std::string ToString() override { return "UpdateProcessesEvent"; }
	};
}