#include "AddGameObjectToCurrentSceneEvent.h"
#include "Events.h"

namespace gamelib
{
	AddGameObjectToCurrentSceneEvent::AddGameObjectToCurrentSceneEvent(const std::shared_ptr<GameObject>& gameObject): Event(AddGameObjectToCurrentSceneEventId)
	{
		SetGameObject(gameObject);
	}

	void AddGameObjectToCurrentSceneEvent::SetGameObject(const std::shared_ptr<GameObject>& gameObject)
	{
		this->gameObjects = gameObject;
	}

	std::shared_ptr<GameObject> AddGameObjectToCurrentSceneEvent::GetGameObject() const
	{
		return gameObjects;
	}

	std::string AddGameObjectToCurrentSceneEvent::ToString()
	{
		return "add_game_object_to_current_scene_event";
	}
}


