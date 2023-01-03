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

	class GameObjectEvent final : public Event
	{
	public:
		GameObjectEvent(std::shared_ptr<GameObject> gameObject, GameObjectEventContext context);

		EventType GetGameObjectType() override;
		std::string ToString() override;
		GameObjectEventContext Context;
		std::shared_ptr<GameObject> GameObject;
	};

	inline const char* ToString(const GameObjectEventContext context)
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

