#pragma once
#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	const static EventId LevelChangedEventTypeEventId(LevelChangedEventType, "LevelChangedEventType");	
	class SceneChangedEvent final : public Event
	{
	public:
		explicit SceneChangedEvent(const int level) : Event(LevelChangedEventTypeEventId)
		{
			SceneId = level;
		}
		int SceneId;
		std::string SceneFilePath;
		std::string ToString() override;
	};
}

