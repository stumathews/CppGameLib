#include "Sprite.h"
#include "SDL.h"
#include "windows.h"

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(uint xPos, uint yPos, uint frameDurationMs, uint totalFrames,  uint framesPerRow, uint framesPerColumn, uint frameWidth,
		uint frame, bool isVisible, SettingsManager& settingsManager,  EventManager& eventManager) 
		: frameDurationMs(100), 
		totalFrames(totalFrames), 
		framesPerRow(framesPerRow), 
		framesPerColumn(framesPerColumn),
		currentFrameNumber(0), 
		frameHeight(64), 
		frameWidth(64),
		startFrameNumber(0), 
		timeLastFrameShown(0),
		GameObject(xPos, yPos, isVisible, settingsManager, eventManager) { }

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
			if(currentFrameNumber >= totalFrames) 
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
				
		SDL_Rect typicalFrame[] = 
		{ 
			{0,0,66,92},
			{66,0,66,93},
			{132,0,66,93},
			{0,93,66,93},
			{66,93,66,93},
			{132,93,71,92},
			{0,186,71,93},
			{71,186,71,93},
			{142,186,70,92},
			{0,279,71,93},
			{71,279,66,97},
		};

		// Set the graphic's viewport
		auto& viewPort = GetGraphic()->GetViewPort();
		viewPort.x = typicalFrame[FrameNumber].x;
		viewPort.y = typicalFrame[FrameNumber].y;
		viewPort.w = typicalFrame[FrameNumber].w;
		viewPort.h = typicalFrame[FrameNumber].h;
	}

	/// <summary>
	/// Name of this class
	/// </summary>
	/// <returns></returns>
	std::string AnimatedSprite::GetName()
	{
		return "AnimatedSprite";
	}
}