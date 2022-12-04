#pragma once
#include "Event.h"
#include <string>
#include <vector>
namespace gamelib
{
	class StartNetworkLevelEvent final : public Event
	{
	public:

		explicit StartNetworkLevelEvent(int level) : Event(EventType::StartNetworkLevel)
		{
			this->level = level;
		}
	
		int level;

	};
}
