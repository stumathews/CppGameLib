#include "StaticSprite.h"
#include <memory>
using namespace std;

namespace gamelib
{
	StaticSprite::StaticSprite(const Coordinate<int> position, const std::shared_ptr<SpriteAsset>
	                           & spriteAsset, const bool isVisible = true)
		: DrawableGameObject(position.GetX(), position.GetY(), isVisible)
	{
		asset = spriteAsset;
		this->IsVisible = isVisible;
	}

	std::shared_ptr<StaticSprite> StaticSprite::Create(const int x, const int y, const std::shared_ptr<SpriteAsset>& spriteAsset)
	{
		auto _sprite = shared_ptr<StaticSprite>(new StaticSprite(gamelib::Coordinate<int>(x, y), spriteAsset));

		// Here we actually set te graphic that the sprite actually uses to show itself
		_sprite->SetGraphic(spriteAsset);
		_sprite->keyFrames = spriteAsset->KeyFrames;
		_sprite->SetFrame(0);
		return _sprite;
	}

	std::shared_ptr<StaticSprite> StaticSprite::Create(const Coordinate<int> coordinate, const std::shared_ptr<
		                                                   SpriteAsset>
	                                                   & spriteAsset)
	{
		return Create(coordinate.GetX(), coordinate.GetY(), spriteAsset);
	}

	std::vector<std::shared_ptr<Event>> StaticSprite::HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs)
	{
		return {};
	}

	void StaticSprite::AdvanceFrame()
	{
		currentFrame = static_cast<uint>(currentFrame) < GetNumKeyFrames() - 1 ? currentFrame + 1 : 0;
		SetFrame(currentFrame);
	}

	void StaticSprite::SetFrame(const uint frameNumber) const
	{
		if (!HasGraphic())
			return;

		// Get the rectangle that defines the viewport
		auto& [x, y, w, h] = GetGraphic()->GetViewPort();

		// Is this a static sprite or a dynamic moving sprite?
		if (!keyFrames.empty())
		{
			// Dynamic: Adjust it to refer to the position of the frame in the picture
			x = keyFrames[frameNumber].X;
			y = keyFrames[frameNumber].Y;
			w = keyFrames[frameNumber].W;
			h = keyFrames[frameNumber].H;
		}
	}

	GameObjectType StaticSprite::GetGameObjectType()
	{
		return GameObjectType::StaticSprite;
	}

	void StaticSprite::LoadSettings()
	{
	}

	void StaticSprite::Draw(SDL_Renderer* renderer)
	{
		DrawableGameObject::Draw(renderer);
	}
}

