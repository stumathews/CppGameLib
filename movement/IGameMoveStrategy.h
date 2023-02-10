#pragma once
#include "pch.h"
#include <memory>
#include <movement/IMovement.h>

class Room;
class Movement;

namespace gamelib
{
	class IGameObjectMoveStrategy
	{
	public:
		virtual ~IGameObjectMoveStrategy() = default;
		/// <summary>
		/// Move the player according to the calculated movement
		/// </summary>
		/// <returns>true if movement could be performed, false otherwise</returns>
		virtual bool MoveGameObject(std::shared_ptr<gamelib::IMovement> movement) = 0;
	};
}
