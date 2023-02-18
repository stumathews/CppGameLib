#pragma once
#include "Event.h"
#include "../net/NetworkPlayer.h"

namespace gamelib
{
	const static EventId NetworkPlayerJoinedEventId(NetworkPlayerJoined, "FetchedPickup");
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
