#pragma once
#include "events/Event.h"
#include <string>

#include "Events.h"

namespace gamelib
{
	const static EventId NetworkTrafficReceivedEventId(NetworkTrafficReceived, "NetworkTrafficReceived");
	class NetworkTrafficReceivedEvent final :  public Event
	{
	public:
		NetworkTrafficReceivedEvent();
		std::string Identifier;
		std::string Message;
		int BytesReceived{};
	};
}

