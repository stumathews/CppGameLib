#pragma once
#include "Event.h"

namespace gamelib
{
	class StartNetworkLevelEvent final : public Event
	{
	public:

		explicit StartNetworkLevelEvent(const int level) : Event(EventType::StartNetworkLevel)
		{
			this->Level = level;
		}
	
		int Level;

	};
}
