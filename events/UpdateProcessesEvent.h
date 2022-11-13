#pragma once
#include <events/Event.h>
namespace gamelib
{
	class UpdateProcessesEvent : public Event
	{
	public:
		UpdateProcessesEvent(unsigned long deltaMs): Event(EventType::UpdateProcesses), deltaMs(deltaMs) {}
		std::string ToString() override { return "UpdateProcessesEvent"; }
		unsigned long deltaMs;
	};
}