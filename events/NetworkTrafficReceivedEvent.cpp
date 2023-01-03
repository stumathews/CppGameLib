#include "NetworkTrafficReceivedEvent.h"

namespace gamelib
{
	NetworkTrafficReceivedEvent::NetworkTrafficReceivedEvent(const EventType eventType) : Event(eventType) 
	{

	}

	EventType NetworkTrafficReceivedEvent::GetGameObjectType()
	{
		return EventType::NetworkTrafficReceived; 
}
	std::string NetworkTrafficReceivedEvent::ToString() 
	{ 
		return "PlayerNetworkTrafficReceived";
	}
}