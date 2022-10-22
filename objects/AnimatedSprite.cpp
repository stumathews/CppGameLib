#include "AnimatedSprite.h"
#include "SDL.h"
#include "windows.h"
#include "exceptions/EngineException.h"
#include "resource/ResourceManager.h"
#include <SpriteAsset.h>

using namespace std;

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(uint xPos, uint yPos, float inFrameDurationMs, bool isVisible, ABCDRectangle dimensions) : DrawableGameObject(xPos, yPos, isVisible) 
	{ 
		frameDurationMs = inFrameDurationMs;
		currentFrameNumber = startFrameNumber = timeLastFrameShown = 0;
		Dimensions = dimensions;
		LoadSettings();
	}

	shared_ptr<AnimatedSprite> AnimatedSprite::Create(int x, int y, std::shared_ptr<SpriteAsset> spriteAsset)
	{
		auto sprite = shared_ptr<AnimatedSprite>(new AnimatedSprite(x, y, spriteAsset->FrameDurationMs, true, spriteAsset->Dimensions));
		
		sprite->SetGraphic(spriteAsset);
		sprite->KeyFrames = spriteAsset->KeyFrames;

		Initialize(sprite);

		return sprite;
	}

	void AnimatedSprite::Initialize(std::shared_ptr<AnimatedSprite> sprite)
	{
		// The sprite will initially be set to the first frame
		sprite->SetAnimationFrame(0);
	}

	/// <summary>
	/// Set game object type
	/// </summary>
	GameObjectType AnimatedSprite::GetGameObjectType()
	{
		return GameObjectType::AnimatedSprite;
	}

	/// <summary>
	/// Draw sprite
	/// </summary>
	/// <param name="renderer"></param>
	void AnimatedSprite::Draw(SDL_Renderer* renderer)
	{
		DrawableGameObject::Draw(renderer);		
	}

	void AnimatedSprite::LoadSettings()
	{
		debug = SettingsManager::Get()->GetBool("sprite", "debug");
	}

	void AnimatedSprite::MoveSprite(int x, int y)
	{
		Position.SetX(x);
		Position.SetY(y);
	}

	/// <summary>
	/// Update sprite
	/// </summary>
	void AnimatedSprite::Update(float deltaMs)
	{
		const unsigned long durationSinceLastFrameMs = timeGetTime() - timeLastFrameShown;
		const auto countKeyFrames = KeyFrames.size();
		
		// Switch to the next frame if we've been on the current frame too long
		if (durationSinceLastFrameMs >= frameDurationMs)
		{
			AdvanceCurrentFrameNumber();

			// Only cycle through supported animation groups
			SkipUnsupportedAnimationGroupFrames();			

			
			SetAnimationFrame(currentFrameNumber);

			timeLastFrameShown = timeGetTime();
		}
	}

	void AnimatedSprite::AdvanceCurrentFrameNumber()
	{
		// Move to the next frame number
		currentFrameNumber++;

		// Need to cycle back to the first frame if we're on the last in the key frames
		if (currentFrameNumber >= KeyFrames.size())
		{
			currentFrameNumber = startFrameNumber;
		}
	}

	void AnimatedSprite::SetSingleFrameDuration(int frameDuration)
	{
		frameDurationMs = frameDuration;
	}

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

	bool AnimatedSprite::IsCurrentFrameInAnimationGroup()
	{
		return KeyFrames[currentFrameNumber].group == animationFrameGroup;
	}

	/// <summary>
	/// Play animation
	/// </summary>
	void AnimatedSprite::PlayAnimation()
	{
		stopped = false;
	}

	/// <summary>
	/// Stop animation
	/// </summary>
	void AnimatedSprite::DisableAnimation()
	{
		stopped = true;
	}

	void AnimatedSprite::EnableAnimation()
	{
		stopped = false;
	}

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
		if (!HasGraphic())
			return;

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

	/// <summary>
	/// Name of this class
	/// </summary>
	/// <returns></returns>
	std::string AnimatedSprite::GetName()
	{
		return "AnimatedSprite";
	}

	void AnimatedSprite::AddKeyFrame(KeyFrame keyFrame)
	{
		KeyFrames.push_back(keyFrame);
	}

	void AnimatedSprite::SetAnimationFrameGroup(std::string group)
	{
		animationFrameGroup = group;
	}

}