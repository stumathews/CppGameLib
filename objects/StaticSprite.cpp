#include "pch.h"
#include "StaticSprite.h"
#include <common/TypeAliases.h>
#include <memory>
using namespace std;

StaticSprite::StaticSprite(gamelib::coordinate<int> startingPosition, std::shared_ptr<gamelib::SpriteAsset> spriteAsset, bool IsVisible = true) 
	: gamelib::DrawableGameObject(startingPosition.GetX(), startingPosition.GetY(), IsVisible )
{
	asset = spriteAsset;
	IsVisible = IsVisible;
}

std::shared_ptr<StaticSprite> StaticSprite::Create(int x, int y, std::shared_ptr<gamelib::SpriteAsset> spriteAsset)
{	
	auto _sprite = shared_ptr<StaticSprite>(new StaticSprite(gamelib::coordinate<int>(x, y), spriteAsset));
	
	// Here we actually set te graphic that the sprite actually uses to show itself
	_sprite->SetGraphic(spriteAsset);
	_sprite->KeyFrames = spriteAsset->KeyFrames;
	_sprite->SetFrame(0);
	return _sprite;
}

std::shared_ptr<StaticSprite> StaticSprite::Create(gamelib::coordinate<int> coordinate, std::shared_ptr<gamelib::SpriteAsset> spriteAsset)
{
	return Create(coordinate.GetX(), coordinate.GetY(), spriteAsset);
}

std::vector<std::shared_ptr<gamelib::Event>> StaticSprite::HandleEvent(std::shared_ptr<gamelib::Event> event)
{	
	return std::vector<std::shared_ptr<gamelib::Event>>();
}

void StaticSprite::AdvanceFrame()
{		
	currentFrame = currentFrame < GetNumKeyFrames() - 1 ? currentFrame + 1 : 0;
	SetFrame(currentFrame);
}

void StaticSprite::SetFrame(gamelib::uint FrameNumber) 
{
	if (!HasGraphic())
		return;

	// Get the rectangle that defines the viewport
	auto& viewPort = GetGraphic()->GetViewPort();

	// Is this a static sprite or a dynamic moving sprite?
	if (KeyFrames.size() > 0)
	{
		// Dynamic: Adjust it to refer to the position of the frame in the picture
		viewPort.x = KeyFrames[FrameNumber].x;
		viewPort.y = KeyFrames[FrameNumber].y;
		viewPort.w = KeyFrames[FrameNumber].w;
		viewPort.h = KeyFrames[FrameNumber].h;
	}
}

gamelib::GameObjectType StaticSprite::GetGameObjectType()
{
    return gamelib::GameObjectType::StaticSprite;
}

void StaticSprite::LoadSettings()
{
}

void StaticSprite::Draw(SDL_Renderer* renderer)
{
	DrawableGameObject::Draw(renderer);
}

