#pragma once

#include <memory>
#include "Event.h"
#include "objects/GameObject.h"

namespace gamelib
{
	class AddGameObjectToCurrentSceneEvent final : public Event
	{
	public:
		AddGameObjectToCurrentSceneEvent(std::shared_ptr<GameObject> game_object, int event_id = 0);

		void set_game_object(std::shared_ptr<GameObject> game_object);

		std::shared_ptr<GameObject> GetGameObject() const;
		std::string to_str() override;

	private:
		std::shared_ptr<GameObject> the_game_object;
	};
}




