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
		frameNumber = static_cast<uint>(frameNumber) < GetNumKeyFrames() - 1 ? frameNumber + 1 : 0;
		SetFrame(frameNumber);
	}

	void StaticSprite::SetFrame(const uint number)
	{
		if (!HasGraphic())
			return;
		frameNumber = static_cast<int>(number);
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
		if (HasGraphic())
		{
			const auto graphicAsset = GetGraphic();

			if (graphicAsset->type == "graphic")
			{
				const auto& frame = keyFrames[frameNumber];
				
				const SDL_Rect srcLocation = { frame.X, frame.Y, frame.W, frame.H };
				const SDL_Rect drawLocation = { Position.GetX(), Position.GetY(), frame.W, frame.H };

				// Copy the texture (restricted by viewport) to the drawLocation on the screen
				SDL_RenderCopy(renderer, graphicAsset->GetTexture(), &srcLocation, &drawLocation);
				
			}
			else
			{
				// Log error here
			}
		}
	}
}

