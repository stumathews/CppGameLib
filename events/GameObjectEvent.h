#pragma once

#include <memory>

#include "events/Event.h"
namespace gamelib
{
	class GameObject;

	enum class GameObjectEventContext
	{
		Remove,
		RemoveSubscription
	};

	class GameObjectEvent : public Event
	{
	public:
		GameObjectEvent(const int event_id, GameObject *gameObject, GameObjectEventContext context ) : Event(EventType::GameObject, event_id), context(context), gameObject(std::move(gameObject))
		{
		}

		EventType GetGameObjectType() override { return EventType::GameObject; }
		std::string to_str() override { return "GameObjectEvent"; }
		GameObjectEventContext context;
		GameObject *gameObject;
	};
}

