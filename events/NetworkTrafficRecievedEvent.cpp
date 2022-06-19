#include "NetworkTrafficRecievedEvent.h"

namespace gamelib
{
	NetworkTrafficRecievedEvent::NetworkTrafficRecievedEvent(gamelib::EventType eventType, int eventId) : Event(eventType, eventId) 
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