#pragma once
#include "Event.h"
namespace gamelib
{
	class SceneLoadedEvent final : public Event
	{
	public:
		explicit SceneLoadedEvent(int sceneId);
		std::string ToString() override;
		int SceneId;
	};
}

