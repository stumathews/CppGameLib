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
		GameObjectEvent(const int event_id, GameObject* gameObject, GameObjectEventContext context);

		EventType GetGameObjectType() override;
		std::string ToString() override;
		GameObjectEventContext context;
		GameObject *gameObject;
	};
}

