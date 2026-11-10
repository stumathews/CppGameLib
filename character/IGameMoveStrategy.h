#pragma once
#include <memory>

class Room;
class Movement;

namespace gamelib
{
	class IMovement;

	// Actually moves or assesses the movement potential GameObject.
	// Can be used by a NPC to move itself (MoveGameObject) or determine if the potential move is invalid (IsValidMove)
	// A move is encoded as a Movement which defines the direction and amount of pixels to move.
	//
	// In most cases the environment is consulted to determine of the move it possible/valid.
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
		virtual bool IsValidMove(const std::shared_ptr<IMovement>& movement) = 0;
		virtual bool CanGameObjectMove(Direction direction) = 0;
	};
}
