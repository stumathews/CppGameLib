#include "GameObjectEvent.h"
#include <events/GameObjectEvent.h>
using namespace std;

namespace gamelib
{

	GameObjectEvent::GameObjectEvent(shared_ptr<GameObject> gameObject, GameObjectEventContext context)
		: Event(EventType::GameObject), context(context), gameObject(std::move(gameObject))
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
