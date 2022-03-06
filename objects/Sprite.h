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
			uint speed, 
			uint totalFrames, 
			uint framesPerRow, 
			uint framesPerColumn, 
			uint frameWidth, 
			uint frame, 
			bool isVisible, 
			SettingsManager& settingsManager,
			EventManager& eventManager);

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
		
	private:
		unsigned long timeLastFrame;
		uint totalFrames;
		uint framesPerRow;
		uint framesPerColumn;
		uint currentFrame;
		uint startFrame;
		float speed;
		uint frameWidth;
		uint frameHeight;
		bool stopped = false;
	};
}
