#pragma once
#include "events/Event.h"
#include <string>

namespace gamelib
{
	class NetworkTrafficReceivedEvent final :  public Event
	{
	public:
		explicit NetworkTrafficReceivedEvent(EventType eventType);
		EventType GetGameObjectType() override;
		std::string ToString() override;
		std::string Identifier;
		std::string Message;
		int BytesReceived{};
	};
}

