#pragma once
#include "GameObject.h"
#include "KeyFrame.h"
#include "scene/ABCDRectangle.h"

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
			bool isVisible, 
			ABCDRectangle dimensions);

		AnimatedSprite() = default;

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
		/// Sets viewable region in the sprite sheet dimensions (used when showing static non-sheet picture)
		/// </summary>
		/// <param name="dimensions"></param>
		void AdjustViewportToCurrentDimensions();

		/// <summary>
		/// Name of the sprite
		/// </summary>
		std::string GetName() override;

		/// <summary>
		/// Add new keyframe
		/// </summary>
		/// <param name="keyFrame"></param>
		void AddKeyFrame(KeyFrame keyFrame);

		/// <summary>
		/// Dimensions of Spritesheet
		/// </summary>
		ABCDRectangle Dimensions;

		/// <summary>
		/// Ordered set of key frames
		/// </summary>
		std::vector<KeyFrame> KeyFrames;
						
	private:
		/// <summary>
		/// Time taken lst frame was shown
		/// </summary>
		unsigned long timeLastFrameShown;
				
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
		/// If the animation has stopped
		/// </summary>
		bool stopped = false;

	};
}
