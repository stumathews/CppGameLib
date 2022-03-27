#include "Sprite.h"
#include "SDL.h"
#include "windows.h"
#include "exceptions/base_exception.h"

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(uint xPos, uint yPos, uint frameDurationMs, bool isVisible, ABCDRectangle dimensions) 
		: frameDurationMs(100), currentFrameNumber(0), startFrameNumber(0), timeLastFrameShown(0), Dimensions(dimensions), GameObject(xPos, yPos, isVisible) 
	{ 
		
	}

	/// <summary>
	/// Set game object type
	/// </summary>
	/// <returns></returns>
	object_type AnimatedSprite::GetGameObjectType()
	{
		return object_type::AnimatedSprite;
	}

	/// <summary>
	/// Draw sprite
	/// </summary>
	/// <param name="renderer"></param>
	void AnimatedSprite::Draw(SDL_Renderer* renderer)
	{
		GameObject::Draw(renderer);
		Update();	// why do we have to do this in the draw function?	I think its because we need to move the key frame/do timer stuff
	}

	/// <summary>
	/// Update sprite
	/// </summary>
	void AnimatedSprite::Update()
	{
		const unsigned long durationSinceLastFrameMs = timeGetTime() - timeLastFrameShown;
		
		// Switch to the next frame if we've been on the current frame too long
		if(durationSinceLastFrameMs >= frameDurationMs) 
		{
			// Record which frame we're on
			currentFrameNumber++;

			// Need to cycle back to the first frame if we're on the last in the key frames
			if(currentFrameNumber >= KeyFrames.size()) 
			{
				currentFrameNumber = startFrameNumber;
			}

			SetAnimationFrame(currentFrameNumber);

			timeLastFrameShown = timeGetTime();		
		}
	}

	/// <summary>
	/// Play animation
	/// </summary>
	void AnimatedSprite::PlayAnimation()
	{
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

	/// <summary>
	/// Set animation frame number
	/// </summary>
	/// <param name="FrameNumber"></param>
	void AnimatedSprite::SetAnimationFrame(uint FrameNumber) const
	{
		if(!HasGraphic())
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

		viewPort = { Dimensions.GetAx(), Dimensions.GetAy(), Dimensions.GetWidth(), Dimensions.GetHeight() };
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
}