#pragma once
#include "Event.h"
#include "../net/NetworkPlayer.h"

namespace gamelib
{
	class NetworkPlayerJoinedEvent : public Event
	{	
	public:
		explicit NetworkPlayerJoinedEvent(NetworkPlayer player) : Event(EventType::NetworkPlayerJoined)
		{
			this->player = player;
		}

		NetworkPlayer player;
	};
}
