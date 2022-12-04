#include "NetworkTrafficRecievedEvent.h"

namespace gamelib
{
	NetworkTrafficRecievedEvent::NetworkTrafficRecievedEvent(gamelib::EventType eventType) : Event(eventType) 
	{

	}

	EventType NetworkTrafficRecievedEvent::GetGameObjectType()
	{
		return EventType::NetworkTrafficReceived; 
}
	std::string NetworkTrafficRecievedEvent::ToString() 
	{ 
		return "PlayerNetworkTrafficRecieved";
	}
}