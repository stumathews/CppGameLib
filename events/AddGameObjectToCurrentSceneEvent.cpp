#include "AddGameObjectToCurrentSceneEvent.h"

namespace gamelib
{
	add_game_object_to_current_scene_event::add_game_object_to_current_scene_event(const std::shared_ptr<game_object> game_object, const int event_id): event(event_type::AddGameObjectToCurrentScene, event_id)
	{
		set_game_object(game_object);
	}

	void add_game_object_to_current_scene_event::set_game_object(const std::shared_ptr<game_object> game_object)
	{
		this->the_game_object = game_object;
	}

	std::shared_ptr<game_object> add_game_object_to_current_scene_event::get_game_object() const
	{
		return the_game_object;
	}

	std::string add_game_object_to_current_scene_event::to_str()
	{
		return "add_game_object_to_current_scene_event";
	}
}


