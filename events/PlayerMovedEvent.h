#pragma once
#include <memory>
#include "Event.h"
#include "PositionChangeEvent.h"
#include "objects/PlayerComponent.h"
namespace gamelib
{
	class PlayerMovedEvent final : public Event
	{
	public:
		explicit PlayerMovedEvent(std::shared_ptr<PlayerComponent> &component, Direction direction) :
			Event(EventType::PlayerMovedEventType), direction(direction), player(component)
		{
		}

		std::shared_ptr<PlayerComponent> get_player_component() const { return player; }
		std::string to_str() override;
		const Direction direction;
	private:
		std::shared_ptr<PlayerComponent> player;
	};
}

