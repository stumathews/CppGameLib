#pragma once
#include <memory>
#include "Event.h"
#include "PositionChangeEvent.h"
#include "objects/player_component.h"
namespace gamelib
{
	class player_moved_event final : public Event
	{
	public:
		explicit player_moved_event(std::shared_ptr<player_component> &component, Direction direction) :
			Event(EventType::PlayerMovedEventType), direction(direction), player(component)
		{
		}

		std::shared_ptr<player_component> get_player_component() const { return player; }
		std::string to_str() override;
		const Direction direction;
	private:
		std::shared_ptr<player_component> player;
	};
}

