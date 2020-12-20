#pragma once

#include <memory>
#include "Event.h"
#include "objects/game_object.h"

namespace gamelib
{
	class add_game_object_to_current_scene_event final : public event
	{
	public:
		add_game_object_to_current_scene_event(std::shared_ptr<game_object> game_object, int event_id = 0);

		void set_game_object(std::shared_ptr<game_object> game_object);

		std::shared_ptr<game_object> get_game_object() const;
		std::string to_str() override;

	private:
		std::shared_ptr<game_object> the_game_object;
	};
}




