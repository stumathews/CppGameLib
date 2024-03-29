#pragma once
#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	const static EventId SceneLoadedEventId(SceneLoaded, "SceneLoaded");
	class SceneLoadedEvent final : public Event
	{
	public:
		explicit SceneLoadedEvent(int sceneId);
		std::string ToString() override;
		int SceneId;
	};
}

