#pragma once
#include <memory>
#include <character/IMovement.h>

class Room;
class Movement;

namespace gamelib
{
	// Makes a movement
	class IGameObjectMoveStrategy
	{
	public:
		IGameObjectMoveStrategy() = default; // Needed
		IGameObjectMoveStrategy(const IGameObjectMoveStrategy& other) = delete;
		IGameObjectMoveStrategy(const IGameObjectMoveStrategy&& other) = delete;
		IGameObjectMoveStrategy& operator=(const IGameObjectMoveStrategy& other) = delete;
		IGameObjectMoveStrategy& operator=(const IGameObjectMoveStrategy&& other) = delete;

		virtual ~IGameObjectMoveStrategy() = default;

		/// <summary>
		/// Move the player according to the calculated movement
		/// </summary>
		/// <returns>true if movement could be performed, false otherwise</returns>
		virtual bool MoveGameObject(std::shared_ptr<IMovement> movement) = 0;
		virtual bool IsValidMove(const std::shared_ptr<gamelib::IMovement>& movement) = 0;
		virtual bool CanGameObjectMove(Direction direction) = 0;
	};
}
