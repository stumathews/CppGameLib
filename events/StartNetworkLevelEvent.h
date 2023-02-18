#pragma once
#include "Event.h"

namespace gamelib
{
	const static EventId StartNetworkLevelEventId(StartNetworkLevel, "StartNetworkLevel");
	class StartNetworkLevelEvent final : public Event
	{
	public:

		explicit StartNetworkLevelEvent(const int level) : Event(StartNetworkLevelEventId)
		{
			this->Level = level;
		}
	
		int Level;

	};
}
