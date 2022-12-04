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
		GameObjectEvent(std::shared_ptr<GameObject> gameObject, GameObjectEventContext context);

		EventType GetGameObjectType() override;
		std::string ToString() override;
		GameObjectEventContext context;
		std::shared_ptr<GameObject> gameObject;
	};

	inline const char* ToString(GameObjectEventContext context)
	{
		switch (context)
		{
			case GameObjectEventContext::Remove:   return "Remove";
			case GameObjectEventContext::RemoveSubscription: return "RemoveSubscription";
			default:      
				return "[Unknown GameObjectEventContext]";
		}
	}
}

