#pragma once
#include "Event.h"
namespace gamelib
{
	class SceneLoadedEvent : public Event
	{
	public:
		SceneLoadedEvent(int scene_id);
		std::string ToString() override;
		int scene_id_;
	};
}

