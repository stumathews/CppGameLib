#include "Npc.h"
#include <memory>
#include "Movement/Movement.h"
#include "objects/AnimatedSprite.h"

gamelib::Npc::Npc(const std::string& name, const std::string& type, const Coordinate<int> position,
                  const bool visible,
                  std::shared_ptr<AnimatedSprite> sprite, std::shared_ptr<IGameObjectMoveStrategy> moveStrategy):
DrawableGameObject(name, type, position, visible), Sprite(std::move(sprite)), moveStrategy(std::move(moveStrategy))
{
	currentMovingDirection = currentFacingDirection = Direction::Down;
	Dimensions = Sprite->Dimensions;
	Hotspot = std::make_shared<gamelib::Hotspot>(Position, Dimensions.GetWidth(), Dimensions.GetHeight(), Dimensions.GetWidth() / 2);
	
	CalculateBounds(Position, Dimensions.GetWidth(), Dimensions.GetHeight());
}

void gamelib::Npc::Draw(SDL_Renderer* renderer)
{
	Sprite->Draw(renderer);
}

gamelib::GameObjectType gamelib::Npc::GetGameObjectType()
{
	return GameObjectType::GameDefined;
}

gamelib::Direction gamelib::Npc::GetDirection() const
{
	return currentFacingDirection;
}

void gamelib::Npc::SwapCurrentDirection()
{
	if(currentFacingDirection == Direction::Up) { SetNpcDirection(Direction::Down);}
	else if(currentFacingDirection == Direction::Left) { SetNpcDirection(Direction::Right);}
	else if(currentFacingDirection == Direction::Right) { SetNpcDirection(Direction::Left);}
	else if(currentFacingDirection == Direction::Down) { SetNpcDirection(Direction::Up);}
}

void gamelib::Npc::SetNpcDirection(const Direction direction) { currentMovingDirection = direction; currentFacingDirection = direction; }
