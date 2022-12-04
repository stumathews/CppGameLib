#pragma once
#include "events/Event.h"
#include <string>

namespace gamelib
{
	class NetworkTrafficRecievedEvent :  public Event
	{
	public:
		NetworkTrafficRecievedEvent(gamelib::EventType eventType);
		virtual EventType GetGameObjectType() override;
		virtual std::string ToString() override;
		std::string Identifier;
		std::string Message;
		int bytesReceived;
	};
}

