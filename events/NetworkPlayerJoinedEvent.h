#pragma once
#include "Event.h"
#include "../net/NetworkPlayer.h"

namespace gamelib
{
	class NetworkPlayerJoinedEvent final : public Event
	{	
	public:
		explicit NetworkPlayerJoinedEvent(const NetworkPlayer& player) : Event(EventType::NetworkPlayerJoined)
		{
			this->Player = player;
		}

		NetworkPlayer Player;
	};
}
