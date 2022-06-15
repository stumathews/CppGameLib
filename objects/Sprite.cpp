#include "Sprite.h"
#include "SDL.h"
#include "windows.h"
#include "exceptions/EngineException.h"
#include "resource/ResourceManager.h"
#include <SpriteAsset.h>

using namespace std;

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(uint xPos, uint yPos, float frameDurationMs, bool isVisible, ABCDRectangle dimensions) : 
		frameDurationMs(frameDurationMs), 
		currentFrameNumber(0), 
		startFrameNumber(0), 
		timeLastFrameShown(0), 
		Dimensions(dimensions),
		DrawableGameObject(xPos, yPos, isVisible) 
	{ 

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
	/// <returns></returns>
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
		//Update(0.0f);	// why do we have to do this in the draw function?	I think its because we need to move the key frame/do timer stuff
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
			// Record which frame we're on
			currentFrameNumber++;

			// Only cycle through supported animation groups
			SkipUnsupportedAnimationGroupFrames();
						
			// Need to cycle back to the first frame if we're on the last in the key frames
			if (currentFrameNumber >= KeyFrames.size())
			{
				currentFrameNumber = startFrameNumber;
			}

			SetAnimationFrame(currentFrameNumber);

			timeLastFrameShown = timeGetTime();
		}
	}

	void AnimatedSprite::SkipUnsupportedAnimationGroupFrames()
	{
		if (currentFrameNumber >= KeyFrames.size())
		{
			currentFrameNumber = startFrameNumber;
		}

		// If we are set to use an animation group name, skip others in the keyframes
		if (!animationFrameGroup.empty())
		{
			// inspect current frame
			bool isGroupFrame = KeyFrames[currentFrameNumber].group == animationFrameGroup;
			while (!isGroupFrame)
			{
				// skip if not a group same as configured group
				currentFrameNumber++;
				if (currentFrameNumber >= KeyFrames.size())
				{
					currentFrameNumber = startFrameNumber;
				}
				// is next frame a group frame?
				isGroupFrame = KeyFrames[currentFrameNumber].group == animationFrameGroup;
			}
			int done = currentFrameNumber;
		}
	}

	/// <summary>
	/// Play animation
	/// </summary>
	void AnimatedSprite::PlayAnimation()
	{
		stopped = false;
		const auto resource = GetGraphic();
		if(!HasGraphic())
		{
			// Asset not loaded yet
			return;
		}

		currentFrameNumber = startFrameNumber;
		SetAnimationFrame(currentFrameNumber);
		timeLastFrameShown = timeGetTime();
	}

	/// <summary>
	/// Stop animation
	/// </summary>
	void AnimatedSprite::StopAnimation()
	{
		stopped = true;
	}

	void AnimatedSprite::StartAnimation()
	{
		stopped = false;
	}

	/// <summary>
	/// Set animation frame number
	/// </summary>
	/// <param name="FrameNumber"></param>
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

		// And change it in-place
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