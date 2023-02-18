#pragma once

#include <memory>

#include "EventNumbers.h"
#include "events/Event.h"

namespace gamelib
{
	class GameObject;

	enum class GameObjectEventContext
	{
		Remove,
		RemoveSubscription
	};
	
	const static EventId GameObjectTypeEventId(GameObjectTypeEvent, "GameObjectTypeEvent");

	class GameObjectEvent final : public Event
	{
	public:
		GameObjectEvent(std::shared_ptr<GameObject> gameObject, GameObjectEventContext context);
		GameObjectEventContext Context;
		std::shared_ptr<GameObject> Object;
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

