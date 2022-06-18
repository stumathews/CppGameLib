#include "GameObjectMover.h"
#include "objects/GameObject.h"
namespace gamelib
{
	GameObjectMover::GameObjectMover(GameObject* gameObject, int moveInterval)
		: gameObject(gameObject), moveInterval(moveInterval)
	{
	}

	void GameObjectMover::SetObject(GameObject* gameObject)
	{
		this->gameObject = gameObject;
	}

	void GameObjectMover::SetMoveInterval(int moveInterval)
	{
		this->moveInterval = moveInterval;
	}

	/// <summary>
	/// Move Up
	/// </summary>
	void GameObjectMover::MoveUp()
	{
		gameObject->Position.SetY(gameObject->Position.GetY() - moveInterval);
	}

	/// <summary>
	/// Move down
	/// </summary>
	void GameObjectMover::MoveDown()
	{
		gameObject->Position.SetY(gameObject->Position.GetY() + moveInterval);
	}

	/// <summary>
	/// Move left
	/// </summary>
	void GameObjectMover::MoveLeft()
	{
		gameObject->Position.SetX(gameObject->Position.GetX() - moveInterval);
	}

	/// <summary>
	/// Move right
	/// </summary>
	void GameObjectMover::MoveRight()
	{
		gameObject->Position.SetX(gameObject->Position.GetX() + moveInterval);
	}
}