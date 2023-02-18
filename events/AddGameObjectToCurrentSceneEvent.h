#pragma once

#include <memory>
#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	class GameObject;

	const static EventId AddGameObjectToCurrentSceneEventId(AddGameObjectToCurrentScene, "AddGameObjectToCurrentSceneEventId");
	/// <summary>
	/// Add a Game Object to scene event
	/// </summary>
	class AddGameObjectToCurrentSceneEvent final : public Event
	{
	public:

		/// <summary>
		/// Create event
		/// </summary>
		explicit AddGameObjectToCurrentSceneEvent(const std::shared_ptr<GameObject>& gameObject);

		/// <summary>
		/// Set game Object
		/// </summary>
		void SetGameObject(const std::shared_ptr<GameObject>& gameObject);

		/// <summary>
		/// Get game Object
		/// </summary>
		[[nodiscard]] std::shared_ptr<GameObject> GetGameObject() const;

		/// <summary>
		/// To string
		/// </summary>
		/// <returns></returns>
		std::string ToString() override;

	private:
		std::shared_ptr<GameObject> gameObjects;
	};
}




