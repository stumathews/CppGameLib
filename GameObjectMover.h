#pragma once

#include "common/aliases.h"

namespace gamelib
{
	class GameObject;
	class GameObjectMover
	{
	public:

		GameObjectMover(GameObject* gameObject, int moveInterval = 10);

		void SetObject(GameObject* gameObject);

		void SetMoveInterval(int moveinterval = 10);

		// Every Game Object can move in its own way
		void MoveUp();
		void MoveDown();
		void MoveLeft();
		void MoveRight();
	private:
		/// <summary>
		/// Each game object has a move internal
		/// </summary>
		int moveInterval;	
		GameObject* gameObject;
	};
}

