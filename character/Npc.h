#pragma once
//#include "character/Hotspot.h"
//#include "character/AnimatedSprite.h"
#include "objects/DrawableGameObject.h"
#include <memory>

#include "Direction.h"
//#include "ai/FSM.h"
//#include "graphic/DrawableText.h"
//#include "character/IGameMoveStrategy.h"

class RoomInfo;
namespace gamelib
{
	class Hotspot;
	class AnimatedSprite;
	class DrawableText;
	class IGameObjectMoveStrategy;

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
		void Update(unsigned long deltaMs) override;
		GameObjectType GetGameObjectType() override;

		// NPCs can turn around
		void InvertCurrentDirection();
		void SetDirection(Direction direction);
		
		// NPCs can have sprites
		AnimatedSpriteSPtr Sprite;

		// NPCs have dimensions
		AbcdRectangle Dimensions{};

		// NPCs also have a hotspot
		HotspotSPtr TheHotspot;

		[[nodiscard]]
		Direction GetDirection() const;

		// NPCs can have their state machine status drawn over them
		DrawableTextSPtr Status = nullptr;

	protected:

		// NPCs have move in a certain way
		GameObjectMoveStrategySPtr gameObjectMoveStrategy;

		// NPCs can have direction
		gamelib::Direction currentMovingDirection;
		gamelib::Direction currentFacingDirection;
	};
}

