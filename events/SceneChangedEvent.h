#pragma once
#include "Events.h"

namespace gamelib
{
	class SceneChangedEvent final : public Event
	{
	public:
		explicit SceneChangedEvent(int level) : Event(EventType::LevelChangedEventType)
		{
			scene_id = level;
		}
		int scene_id;
		std::string scene_file_path;
		std::string to_str() override;
	};
}

