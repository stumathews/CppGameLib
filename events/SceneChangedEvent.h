#pragma once
#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	const static EventId SceneChangedEventTypeEventId(SceneChangedEventType, "SceneChangedEventType");
	class SceneChangedEvent final : public Event
	{
	public:
		explicit SceneChangedEvent(const int level) : Event(SceneChangedEventTypeEventId)
		{
			SceneId = level;
		}
		int SceneId;
		std::string SceneFilePath;
		std::string ToString() override;
	};
}

