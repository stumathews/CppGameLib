#pragma once
#include "GameObject.h"
#include "KeyFrame.h"
#include "scene/ABCDRectangle.h"
#include "objects/DrawableGameObject.h"

namespace gamelib
{
	class SpriteAsset;

	/// <summary>
	/// Sprite sheet based sprite
	/// </summary>
	class AnimatedSprite final : public DrawableGameObject
	{	
	public:

		/// <summary>
		/// Create a Sprite
		/// </summary>
		AnimatedSprite(uint x, uint y, float frameDuration, bool isVisible, ABCDRectangle dimensions);

		AnimatedSprite() = default;

		static std::shared_ptr<AnimatedSprite> Create(int x, int y, std::shared_ptr<SpriteAsset> asset);

		static void Initialize(std::shared_ptr<AnimatedSprite> animatedSprite);

		/// <summary>
		/// Set Game Object Type
		/// </summary>
		/// <returns></returns>
		GameObjectType GetGameObjectType() override;

		/// <summary>
		/// Draw sprite
		/// </summary>
		/// <param name="renderer"></param>
		void Draw(SDL_Renderer* renderer) override;	

		void LoadSettings() override;

		void MoveSprite(int x, int y);
		
		/// <summary>
		/// Update sprite
		/// </summary>
		void Update(float deltaMs) override;
		void AdvanceCurrentFrameNumber();
		void SkipUnsupportedAnimationGroupFrames();

		bool IsCurrentFrameInAnimationGroup();

		void SetSingleFrameDuration(int frameDuration);
		
		/// <summary>
		/// Play animation
		/// </summary>
		void PlayAnimation();

		/// <summary>
		/// Stop animation
		/// </summary>
		void StopAnimation();

		void StartAnimation();

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

		/// <summary>
		/// Only cycle through specific group frames
		/// </summary>
		/// <param name="group"></param>
		void SetAnimationFrameGroup(std::string group);
						
	private:

		std::string animationFrameGroup;

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

		bool debug = false;

	};
}