#include "AnimatedSprite.h"
#include "SDL.h"
#include "windows.h"
#include "exceptions/EngineException.h"
#include "resource/ResourceManager.h"
#include <SpriteAsset.h>

using namespace std;

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(coordinate<int> position, float inFrameDurationMs, bool IsVisible, ABCDRectangle dimensions) : DrawableGameObject(position, IsVisible)
	{ 
		frameDurationMs = inFrameDurationMs;
		currentFrameNumber = startFrameNumber = deltaTime = 0;
		Dimensions = dimensions;
		LoadSettings();
	}

	shared_ptr<AnimatedSprite> AnimatedSprite::Create(coordinate<int> position, std::shared_ptr<SpriteAsset> spriteAsset)
	{
		auto _sprite = shared_ptr<AnimatedSprite>(new AnimatedSprite(position, spriteAsset->FrameDurationMs, true, spriteAsset->Dimensions));
		
		_sprite->SetGraphic(spriteAsset);
		_sprite->KeyFrames = spriteAsset->KeyFrames;

		Initialize(_sprite);

		return _sprite;
	}


	void AnimatedSprite::Update(unsigned long deltaMs, string animationGroup) { SetAnimationFrameGroup(animationGroup); Update(deltaMs); }

	void AnimatedSprite::Update(float deltaMs)
	{
		const unsigned long durationSinceLastFrameMs = timeGetTime() - deltaTime;
		const auto countKeyFrames = KeyFrames.size();
		
		// Switch to the next frame if we've been on the current frame too long
		if (durationSinceLastFrameMs >= frameDurationMs)
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

	void AnimatedSprite::SetSingleFrameDuration(int frameDuration) { frameDurationMs = frameDuration; }

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

	void AnimatedSprite::Initialize(std::shared_ptr<AnimatedSprite> _sprite) { _sprite->SetAnimationFrame(0); }
	void AnimatedSprite::Draw(SDL_Renderer* renderer) { DrawableGameObject::Draw(renderer); }
	void AnimatedSprite::LoadSettings() { debug = SettingsManager::Get()->GetBool("sprite", "debug"); }
	void AnimatedSprite::MoveSprite(int x, int y) { Position.SetX(x); Position.SetY(y); }
	bool AnimatedSprite::IsCurrentFrameInAnimationGroup() { return KeyFrames[currentFrameNumber].group == animationFrameGroup; }
	void AnimatedSprite::PlayAnimation() { stopped = false; }
	void AnimatedSprite::DisableAnimation() { stopped = true; }
	void AnimatedSprite::EnableAnimation() { stopped = false; }
	std::string AnimatedSprite::GetName() { return "AnimatedSprite"; }
	void AnimatedSprite::AddKeyFrame(KeyFrame keyFrame) { KeyFrames.push_back(keyFrame); }
	void AnimatedSprite::SetAnimationFrameGroup(std::string group) { animationFrameGroup = group; }

	/// <summary>
	/// Set animation frame number. This is the keyframe which will be drawn
	/// </summary>
	void AnimatedSprite::SetAnimationFrame(uint FrameNumber) const
	{
		if(!HasGraphic() || stopped)
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

	void AnimatedSprite::AdjustViewportToCurrentDimensions()
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