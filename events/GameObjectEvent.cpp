#include "GameObjectEvent.h"
#include <events/GameObjectEvent.h>
namespace gamelib
{

	GameObjectEvent::GameObjectEvent(const int event_id, GameObject* gameObject, GameObjectEventContext context)
		: Event(EventType::GameObject, event_id), context(context), gameObject(std::move(gameObject))
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
