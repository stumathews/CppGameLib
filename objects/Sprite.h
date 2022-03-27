#pragma once
#include "GameObject.h"

namespace gamelib
{
	/// <summary>
	/// Sprite sheet based sprite
	/// </summary>
	class AnimatedSprite final : public GameObject
	{	
	public:

		/// <summary>
		/// Create a Sprite
		/// </summary>
		AnimatedSprite(uint x, 
			uint y, 
			uint frameDuration, 
			uint totalFrames, 
			uint framesPerRow, 
			uint framesPerColumn, 
			uint frameWidth, 
			uint frame, 
			bool isVisible);

		/// <summary>
		/// Set Game Object Type
		/// </summary>
		/// <returns></returns>
		object_type GetGameObjectType() override;

		/// <summary>
		/// Draw sprite
		/// </summary>
		/// <param name="renderer"></param>
		void Draw(SDL_Renderer* renderer) override;			
		
		/// <summary>
		/// Update sprite
		/// </summary>
		void Update() override;	
		
		/// <summary>
		/// Play animation
		/// </summary>
		void PlayAnimation();

		/// <summary>
		/// Stop animation
		/// </summary>
		void StopAnimation();

		/// <summary>
		/// Set animation Frame
		/// </summary>
		/// <param name="FrameNumber"></param>
		void SetAnimationFrame(uint FrameNumber) const;

		/// <summary>
		/// Name of the sprite
		/// </summary>
		std::string GetName() override;
		
	private:
		/// <summary>
		/// Time taken lst frame was shown
		/// </summary>
		unsigned long timeLastFrameShown;

		/// <summary>
		/// Total frames in sprite sheet
		/// </summary>
		uint totalFrames;

		/// <summary>
		/// Total key frames in a row in sprite sheet
		/// </summary>
		uint framesPerRow;

		/// <summary>
		/// Total frames in a column in sprite sheet
		/// </summary>
		uint framesPerColumn;

		/// <summary>
		/// Current frame number
		/// </summary>
		uint currentFrameNumber;

		/// <summary>
		/// Starting frame number
		/// </summary>
		uint startFrameNumber;

		/// <summary>
		/// The duration of a single animation key frame
		/// </summary>
		float frameDurationMs;

		/// <summary>
		/// An animation Frame height
		/// </summary>
		uint frameWidth;

		/// <summary>
		/// An animation Frame width
		/// </summary>
		uint frameHeight;

		/// <summary>
		/// If the animation has stopped
		/// </summary>
		bool stopped = false;
	};
}
