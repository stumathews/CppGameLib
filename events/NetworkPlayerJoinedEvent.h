#pragma once
#include "Event.h"
#include "EventNumbers.h"
#include "../net/NetworkPlayer.h"

namespace gamelib
{
	const static EventId NetworkPlayerJoinedEventId(NetworkPlayerJoined, "NetworkPlayerJoinedEvent");
	class NetworkPlayerJoinedEvent final : public Event
	{	
	public:
		explicit NetworkPlayerJoinedEvent(const NetworkPlayer& player) : Event(NetworkPlayerJoinedEventId)
		{
			this->Player = player;
		}

		NetworkPlayer Player;
	};
}
