#include "GameObjectEvent.h"
using namespace std;

namespace gamelib
{

	GameObjectEvent::GameObjectEvent(shared_ptr<gamelib::GameObject> gameObject, const GameObjectEventContext context)
		: Event(EventType::GameObject), Context(context), GameObject(std::move(gameObject))
	{
	}

	EventType GameObjectEvent::GetGameObjectType() 
	{
		return EventType::GameObject;
	}
	std::string GameObjectEvent::ToString() { 
		return "GameObjectEvent";
	}
}
