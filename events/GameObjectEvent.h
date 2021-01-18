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

	class GameObjectEvent : public event
	{
	public:
		GameObjectEvent(const int event_id, GameObject *gameObject, GameObjectEventContext context ) : event(event_type::GameObject, event_id), context(context), game_object(std::move(gameObject))
		{
		}

		event_type get_type() override { return event_type::GameObject; }
		std::string to_str() override { return "GameObjectEvent"; }
		GameObjectEventContext context;
		GameObject *game_object;
	};
}

