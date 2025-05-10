#pragma once
#include "character/Hotspot.h"
#include "character/AnimatedSprite.h"
#include "objects/DrawableGameObject.h"
#include <memory>

#include "ai/FSM.h"
#include "graphic/DrawableText.h"
#include "character/IGameMoveStrategy.h"

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
		void Update(const unsigned long deltaMs) override;
		void SwapCurrentDirection();
		void SetNpcDirection(Direction direction);
		
		GameObjectType GetGameObjectType() override;

		// Fields
		std::shared_ptr<AnimatedSprite> Sprite;
		AbcdRectangle Dimensions{};
		std::shared_ptr<Hotspot> Hotspot;

		[[nodiscard]] Direction GetDirection() const;
		std::shared_ptr<DrawableText> Status = nullptr;

	protected:		
		std::shared_ptr<IGameObjectMoveStrategy> gameObjectMoveStrategy;
		FSM stateMachine;
		Direction currentMovingDirection;
		Direction currentFacingDirection;
		FSMState hitWallState;
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

