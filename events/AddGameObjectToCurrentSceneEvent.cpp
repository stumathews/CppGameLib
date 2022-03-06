#include "AddGameObjectToCurrentSceneEvent.h"

namespace gamelib
{
	AddGameObjectToCurrentSceneEvent::AddGameObjectToCurrentSceneEvent(const std::shared_ptr<GameObject> game_object, const int event_id): Event(EventType::AddGameObjectToCurrentScene, event_id)
	{
		set_game_object(game_object);
	}

	void AddGameObjectToCurrentSceneEvent::set_game_object(const std::shared_ptr<GameObject> game_object)
	{
		this->the_game_object = game_object;
	}

	std::shared_ptr<GameObject> AddGameObjectToCurrentSceneEvent::GetGameObject() const
	{
		return the_game_object;
	}

	std::string AddGameObjectToCurrentSceneEvent::to_str()
	{
		return "add_game_object_to_current_scene_event";
	}
}


