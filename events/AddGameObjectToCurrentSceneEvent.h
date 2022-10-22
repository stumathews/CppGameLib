#pragma once

#include <memory>
#include "Event.h"

namespace gamelib
{
	class GameObject;

	/// <summary>
	/// Add a Game Object to scene event
	/// </summary>
	class AddGameObjectToCurrentSceneEvent final : public Event
	{
	public:

		/// <summary>
		/// Create event
		/// </summary>
		/// <param name="game_object"></param>
		/// <param name="event_id"></param>
		AddGameObjectToCurrentSceneEvent(std::shared_ptr<GameObject> gameObject, int eventid = 0);

		/// <summary>
		/// Set game Object
		/// </summary>
		void SetGameObject(std::shared_ptr<GameObject> gameObject);

		/// <summary>
		/// Get game Object
		/// </summary>
		std::shared_ptr<GameObject> GetGameObject() const;

		/// <summary>
		/// To string
		/// </summary>
		/// <returns></returns>
		std::string ToString() override;

	private:
		std::shared_ptr<GameObject> gameObjects;
	};
}




