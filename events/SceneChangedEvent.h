#pragma once
#include "Events.h"

namespace gamelib
{
	class scene_changed_event final : public event
	{
	public:
		explicit scene_changed_event(int level) : event(event_type::LevelChangedEventType)
		{
			scene_id = level;
		}
		int scene_id;
		std::string scene_file_path;
		std::string to_str() override;
	};
}

