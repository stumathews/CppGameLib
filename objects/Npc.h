#pragma once
#include "Hotspot.h"
#include "objects/AnimatedSprite.h"
#include "objects/DrawableGameObject.h"
#include <memory>

#include "ai/FSM.h"
#include "movement/IGameMoveStrategy.h"

class RoomInfo;
namespace gamelib
{
	class Npc : public DrawableGameObject
	{
	public:
		Npc(const std::string& name, const std::string& type, Coordinate<int> position, bool visible,
		    std::shared_ptr<AnimatedSprite> sprite,
		    std::shared_ptr<IGameObjectMoveStrategy> moveStrategy = nullptr);
					
		void Draw(SDL_Renderer* renderer) override;
		GameObjectType GetGameObjectType() override { return GameObjectType::GameDefined;}
		void SwapCurrentDirection();
		void SetNpcDirection(Direction direction);

		// Fields
		std::shared_ptr<AnimatedSprite> Sprite;
		ABCDRectangle Dimensions{};
		std::shared_ptr<Hotspot> Hotspot;

	protected:		
		std::shared_ptr<IGameObjectMoveStrategy> moveStrategy;
		FSM stateMachine;
		Direction currentMovingDirection;
		Direction currentFacingDirection;
		FSMState HitWallState;
		FSMState upState;
		FSMState downState;
		FSMState leftState;
		FSMState rightState;
		FSMTransition invalidMoveTransition;
		FSMTransition onUpDirection;
		FSMTransition onDownDirection;
		FSMTransition onLeftDirection;
		FSMTransition onRightDirection;
	};
}

