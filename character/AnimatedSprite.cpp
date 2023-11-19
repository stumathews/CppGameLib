#include "AnimatedSprite.h"
#include "SDL.h"
#include "resource/ResourceManager.h"
#include <asset/SpriteAsset.h>
#include "character/Direction.h"
#include "exceptions/EngineException.h"
#include "file/SettingsManager.h"
#include <Windows.h>

using namespace std;

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(const std::string& name, const std::string& type, const Coordinate<int> position,
	                               const float frameDuration, const bool isVisible, const shared_ptr<SpriteAsset>&
	                               spriteAsset) 
		: DrawableGameObject(name, type, position, isVisible)
	{
		Asset = spriteAsset;
		frameDurationMs = frameDuration;
		currentFrameNumber = startFrameNumber = deltaTime = 0;
		Dimensions = spriteAsset->Dimensions;
		LoadSettings();
	}

	shared_ptr<AnimatedSprite> AnimatedSprite::Create(const Coordinate<int> position, const std::shared_ptr<SpriteAsset>& asset)
	{
		auto sprite = std::make_shared<AnimatedSprite>("", "", position, asset->FrameDurationMs, true, asset);
		
		sprite->SetGraphic(asset);
		sprite->KeyFrames = asset->KeyFrames;

		Initialize();

		return sprite;
	}


	void AnimatedSprite::Update(const unsigned long deltaMs, const string& animationGroup)
	{
		SetAnimationFrameGroup(animationGroup);
		Update(deltaMs);
	}


	void AnimatedSprite::Update(const unsigned long deltaMs)
	{
		const unsigned long durationSinceLastFrameMs = timeGetTime() - deltaTime;
		
		// Switch to the next frame if we've been on the current frame too long
		if (static_cast<float>(durationSinceLastFrameMs) >= frameDurationMs)
		{
			AdvanceCurrentFrameNumber();
			SkipUnsupportedAnimationGroupFrames(); // Only cycle through supported animation groups

			deltaTime = timeGetTime();
		}
	}

	void AnimatedSprite::AdvanceCurrentFrameNumber()
	{		
		currentFrameNumber++; // Move to the next frame number

		// Need to cycle back to the first frame if we're on the last in the key frames
		if (currentFrameNumber >= KeyFrames.size())
		{
			currentFrameNumber = startFrameNumber;
		}
	}

	void AnimatedSprite::SetSingleFrameDuration(const int frameDuration) { frameDurationMs = static_cast<float>(frameDuration); }

	void AnimatedSprite::SkipUnsupportedAnimationGroupFrames()
	{
		if (!animationFrameGroup.empty())
		{			
			bool isGroupFrame = IsCurrentFrameInAnimationGroup();
			while (!isGroupFrame)
			{
				AdvanceCurrentFrameNumber();
				isGroupFrame = IsCurrentFrameInAnimationGroup();
			}
		}
	}

	void AnimatedSprite::Initialize() {  }
	void AnimatedSprite::Draw(SDL_Renderer* renderer)
	{
		if (HasGraphic())
		{
			const auto graphicAsset = GetGraphic();

			if (graphicAsset->Type == "graphic")
			{
				const auto& frame = KeyFrames[currentFrameNumber];
				const SDL_Rect srcLocation = { frame.X, frame.Y, frame.W, frame.H  };
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
	void AnimatedSprite::LoadSettings() { debug = SettingsManager::Get()->GetBool("sprite", "debug"); }
	void AnimatedSprite::MoveSprite(const int x, const int y) { Position.SetX(x); Position.SetY(y); }
	void AnimatedSprite::MoveSprite(const Coordinate<int> position) { Position = position; }
	bool AnimatedSprite::IsCurrentFrameInAnimationGroup() const { return KeyFrames[currentFrameNumber].Group == animationFrameGroup; }
	void AnimatedSprite::PlayAnimation() { stopped = false; }
	void AnimatedSprite::DisableAnimation() { stopped = true; }
	void AnimatedSprite::EnableAnimation() { stopped = false; }
	std::string AnimatedSprite::GetName() { return "AnimatedSprite"; }
	void AnimatedSprite::AddKeyFrame(const KeyFrame& keyFrame) { KeyFrames.push_back(keyFrame); }
	void AnimatedSprite::SetAnimationFrameGroup(const std::string& group) { animationFrameGroup = group; }

	std::string AnimatedSprite::GetStdDirectionAnimationFrameGroup(const Direction facingDirection)
	{
		switch (facingDirection)
		{
			case Direction::Up: return "up";
			case Direction::Right: return "right";
			case Direction::Down: return "down";
			case Direction::Left: return "left";
			case Direction::None: THROW(0, "Invalid Direction", "Player");
			default: THROW(0, "Invalid Direction", "Player");  // NOLINT(clang-diagnostic-covered-switch-default)
		}
	}
}