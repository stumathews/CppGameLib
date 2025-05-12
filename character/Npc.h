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
		using AnimatedSpriteSPtr = std::shared_ptr<AnimatedSprite>;
		using HotspotSPtr = std::shared_ptr<Hotspot>;
		using DrawableTextSPtr = std::shared_ptr<DrawableText>;
		using GameObjectMoveStrategySPtr = std::shared_ptr<IGameObjectMoveStrategy>;

		Npc(const std::string& name, const std::string& type, Coordinate<int> position, bool visible,
		    AnimatedSpriteSPtr sprite,
		    GameObjectMoveStrategySPtr moveStrategy = nullptr);

		// NPCs override certain behavior:
		void Draw(SDL_Renderer* renderer) override;
		void Update(const unsigned long deltaMs) override;
		GameObjectType GetGameObjectType() override;

		// NPCs can turn around
		void InvertCurrentDirection();
		void SetDirection(Direction direction);
		
		// NPCs can have sprites
		AnimatedSpriteSPtr Sprite;

		// NPCs have dimensions
		AbcdRectangle Dimensions{};

		// NPCs also have a hotspot
		HotspotSPtr Hotspot;

		[[nodiscard]]
		Direction GetDirection() const;

		// NPCs can have their state machine status drawn over them
		DrawableTextSPtr Status = nullptr;

	protected:

		// NPCs have move in a certain way
		GameObjectMoveStrategySPtr gameObjectMoveStrategy;

		// NPCs have a state machine which they can configure statically
		FSM stateMachine;

		// NPCs can have direction
		Direction currentMovingDirection;
		Direction currentFacingDirection;

		// NPC static states (shared by all NPCs in the Mazer game)
		FSMState hitWallState;
		FSMState upState;
		FSMState downState;
		FSMState leftState;
		FSMState rightState;

		// NPC state transitions
		FSMTransition invalidMoveTransition;
		FSMTransition onUpDirection;
		FSMTransition onDownDirection;
		FSMTransition onLeftDirection;
		FSMTransition onRightDirection;
	};
}

