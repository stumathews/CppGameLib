#include "AnimatedSprite.h"
#include "SDL.h"
#include "resource/ResourceManager.h"
#include <asset/SpriteAsset.h>
#include "character/Direction.h"
#include "exceptions/EngineException.h"
#include "file/SettingsManager.h"
#include <time/time.h>

using namespace std;

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(const std::string& name, const std::string& type, const Coordinate<float> position,
	                               const float frameDuration, const bool isVisible, const shared_ptr<SpriteAsset>&
	                               spriteAsset) 
		: DrawableGameObject(name, type, position, isVisible), frameDurationMs(frameDuration),
		  Asset(spriteAsset), Dimensions(spriteAsset->Dimensions)
	{
		LoadSettings();
		SetGraphic(Asset);
		KeyFrames = Asset->KeyFrames;
		Initialize();
	}

	shared_ptr<AnimatedSprite> AnimatedSprite::Create(const Coordinate<float> position, const std::shared_ptr<SpriteAsset>& asset)
	{
		auto sprite = std::make_shared<AnimatedSprite>("", "", position, asset->FrameDurationMs, true, asset);
		
		sprite->SetGraphic(asset);
		sprite->KeyFrames = asset->KeyFrames;

		sprite->Initialize();

		return sprite;
	}

	void AnimatedSprite::Update(const gamelib::DeltaTime* deltaTime, const string& animationGroup)
	{
		SetAnimationFrameGroup(animationGroup);
		Update(deltaTime);
	}


	void AnimatedSprite::Update(const DeltaTime* deltaTime)
	{
		if (!initialized)
		{
			return; // We won't update until the animated sprite has been initialized
		}

		durationSinceLastFrameMs += deltaTime->milliseconds;

		if (stopped)
		{
			return; // effectively disables animation by not changing key frames
		}

		// Switch to the next frame if we've been on the current frame too long
		if (durationSinceLastFrameMs >= frameDurationMs)
		{
			AdvanceCurrentFrameNumber();
			SkipUnsupportedAnimationGroupFrames(); // Only cycle through supported animation groups
			durationSinceLastFrameMs = 0;
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
		AssertContainsAValidKeyFrameGroup();

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

	void AnimatedSprite::Initialize()
	{
		if (KeyFrames.empty())
		{
			THROW(2, "KeyFrames cannot be empty", "AnimatedSprite");
		}

		AssertContainsAValidKeyFrameGroup();

		if (!initialized)
		{
			initialized = true;
		}
	}
	void AnimatedSprite::Draw(SDL_Renderer* renderer)
	{
		if (HasGraphic())
		{
			const auto graphicAsset = GetGraphic();

			if (graphicAsset->Type == "graphic")
			{
				const auto& frame = KeyFrames[currentFrameNumber];
				const SDL_Rect srcLocation = { frame.X, frame.Y, frame.W, frame.H  };
				const SDL_Rect drawLocation = { static_cast<int>(Position.GetX()), static_cast<int>(Position.GetY()), frame.W, frame.H };

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
	void AnimatedSprite::MoveSprite(const float x, const float y) { Position.SetX(x); Position.SetY(y); }
	void AnimatedSprite::MoveSprite(const Coordinate<float> position) { Position = position; }
	bool AnimatedSprite::IsCurrentFrameInAnimationGroup() const
	{
		return KeyFrames[currentFrameNumber].Group == animationFrameGroup;
	}
	void AnimatedSprite::PlayAnimation() { stopped = false; }
	void AnimatedSprite::DisableAnimation() { stopped = true; }
	void AnimatedSprite::EnableAnimation() { stopped = false; }
	std::string AnimatedSprite::GetName() { return "AnimatedSprite"; }
	void AnimatedSprite::AddKeyFrame(const KeyFrame& keyFrame) { KeyFrames.push_back(keyFrame); }

	bool AnimatedSprite::IsStopped()
	{
		return stopped;
	}

	void AnimatedSprite::AssertContainsAValidKeyFrameGroup()
	{
		if (!containsAValidKeyFrameGroup)
		{
			// This check only runs once
			for (auto keyFrame: KeyFrames)
			{
				if (keyFrame.Group == GetStdDirectionAnimationFrameGroup(Direction::Up) ||
					keyFrame.Group == GetStdDirectionAnimationFrameGroup(Direction::Down) ||
					keyFrame.Group == GetStdDirectionAnimationFrameGroup(Direction::Left)||
					keyFrame.Group == GetStdDirectionAnimationFrameGroup(Direction::Right)) {
					containsAValidKeyFrameGroup = true;
				}
			}

			if (!containsAValidKeyFrameGroup)
			{
				THROW(1, "AnimatedSprite contains no valid Animation Groups. Must contain at least one of 'up', 'down', 'left or 'right'", "AnimatedSprite");
			}
		}
	}

	void AnimatedSprite::SetAnimationFrameGroup(const std::string& group)
	{
		// Before setting the animation group, check that we're setting it to a supported value
		animationFrameGroup = group;
	}

	std::string AnimatedSprite::GetStdDirectionAnimationFrameGroup(const Direction facingDirection)
	{
		switch (facingDirection)
		{
			case Direction::Up: return "up";
			case Direction::Right: return "right";
			case Direction::Down: return "down";
			case Direction::Left: return "left";
			case Direction::None: return "none";
			default: THROW(0, "Invalid Direction", "Player");  // NOLINT(clang-diagnostic-covered-switch-default)
		}
	}

	int AnimatedSprite::GetCurrentKeyFrameIndex() const
	{
		return currentFrameNumber;
	}
}
