#pragma once
#include "Events.h"

namespace gamelib
{
	class SceneChangedEvent final : public Event
	{
	public:
		explicit SceneChangedEvent(const int level) : Event(EventType::LevelChangedEventType)
		{
			SceneId = level;
		}
		int SceneId;
		std::string SceneFilePath;
		std::string ToString() override;
	};
}

