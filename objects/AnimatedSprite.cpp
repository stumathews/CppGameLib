#include "AnimatedSprite.h"
#include "SDL.h"
#include "windows.h"
#include "resource/ResourceManager.h"
#include <SpriteAsset.h>

#include "Direction.h"
#include "exceptions/EngineException.h"
#include "util/SettingsManager.h"

using namespace std;

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(const std::string& name, const std::string& type, const Coordinate<int> position,
	                               const float frameDuration, const bool isVisible, const shared_ptr<SpriteAsset> spriteAsset) 
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
		auto _sprite = std::make_shared<AnimatedSprite>("", "", position, asset->FrameDurationMs, true, asset);
		
		_sprite->SetGraphic(asset);
		_sprite->KeyFrames = asset->KeyFrames;

		Initialize(_sprite);

		return _sprite;
	}


	void AnimatedSprite::Update(const unsigned long deltaMs, const string& animationGroup)
	{
		SetAnimationFrameGroup(animationGroup);
		Update(static_cast<float>(deltaMs));
	}


	void AnimatedSprite::Update(float deltaMs)
	{
		const unsigned long durationSinceLastFrameMs = timeGetTime() - deltaTime;
		
		// Switch to the next frame if we've been on the current frame too long
		if (static_cast<float>(durationSinceLastFrameMs) >= frameDurationMs)
		{
			AdvanceCurrentFrameNumber();
			SkipUnsupportedAnimationGroupFrames(); // Only cycle through supported animation groups
			SetAnimationFrame(currentFrameNumber);

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

	void AnimatedSprite::Initialize(const std::shared_ptr<AnimatedSprite>& _sprite) { _sprite->SetAnimationFrame(0); }
	void AnimatedSprite::Draw(SDL_Renderer* renderer) { DrawableGameObject::Draw(renderer); }
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
			case Direction::None: THROW(0, "Invalid Direction", "Player")
			default: THROW(0, "Invalid Direction", "Player")  // NOLINT(clang-diagnostic-covered-switch-default)
		}
	}

	/// <summary>
	/// Set animation frame number. This is the keyframe which will be drawn
	/// </summary>
	void AnimatedSprite::SetAnimationFrame(const uint FrameNumber) const
	{
		if(!HasGraphic() || stopped)
			return;		
		
		// Get the rectangle that defines the viewport
		auto& viewPort = GetGraphic()->GetViewPort();

		// Is this a static sprite or a dynamic moving sprite?
		if (static_cast<int>(KeyFrames.size()) > 0)
		{
			// Dynamic: Adjust it to refer to the position of the frame in the picture
			viewPort.x = KeyFrames[FrameNumber].X;
			viewPort.y = KeyFrames[FrameNumber].Y;
			viewPort.w = KeyFrames[FrameNumber].W;
			viewPort.h = KeyFrames[FrameNumber].H;
		}
	}

	void AnimatedSprite::AdjustViewportToCurrentDimensions() const
	{
		if (!HasGraphic()) { return; }

		// Get the rectangle that defines the viewport
		auto& viewPort = GetGraphic()->GetViewPort();

		// Update viewport
		viewPort = 
		{ 
			Dimensions.GetAx(), 
			Dimensions.GetAy(),
			Dimensions.GetWidth(),
			Dimensions.GetHeight() 
		};
	}



}