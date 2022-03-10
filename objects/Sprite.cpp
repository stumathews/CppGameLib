#include "Sprite.h"
#include "SDL.h"
#include "windows.h"

namespace gamelib
{
	AnimatedSprite::AnimatedSprite(uint xPos, 
		uint yPos,
		uint speed,
		uint totalFrames, 
		uint framesPerRow, 
		uint framesPerColumn, 
		uint frameWidth,
		uint frame,
		bool isVisible,
		SettingsManager& settingsManager, 
		EventManager& eventManager) 
		: speed(100), totalFrames(totalFrames), framesPerRow(framesPerRow), framesPerColumn(framesPerColumn), currentFrame(0), 
		frameHeight(64), frameWidth(64), startFrame(0), timeLastFrame(0),
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
		Update();	// why do we have to do this in the draw function?	
	}

	/// <summary>
	/// Update sprite
	/// </summary>
	void AnimatedSprite::Update()
	{
		const unsigned long timeSinceLastFrame = timeGetTime() - timeLastFrame;
		if(timeSinceLastFrame >= speed) 
		{
			currentFrame++;
			if(currentFrame >= totalFrames) 
			{
				currentFrame = startFrame;
			}

			SetAnimationFrame(currentFrame);

			timeLastFrame = timeGetTime();		
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

		currentFrame = startFrame;
		SetAnimationFrame(currentFrame);
		timeLastFrame = timeGetTime();
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
	std::string AnimatedSprite::GetName()
	{
		return "AnimatedSprite";
	}
}