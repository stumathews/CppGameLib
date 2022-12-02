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
		AnimatedSprite(gamelib::coordinate<int> position, float frameDuration, bool IsVisible, ABCDRectangle dimensions);
		AnimatedSprite() = default;
		static std::shared_ptr<AnimatedSprite> Create(gamelib::coordinate<int> position, std::shared_ptr<SpriteAsset> asset);
		static void Initialize(std::shared_ptr<AnimatedSprite> animatedSprite);
		/// <summary>
		/// Set game object type
		/// </summary>
		GameObjectType GetGameObjectType() override { return GameObjectType::AnimatedSprite; }
		void Draw(SDL_Renderer* renderer) override;	
		void LoadSettings() override;
		void MoveSprite(int x, int y);
		void Update(float deltaMs) override;
		void Update(unsigned long deltaMs, std::string animationGroup);
		void AdvanceCurrentFrameNumber();
		void SkipUnsupportedAnimationGroupFrames();
		bool IsCurrentFrameInAnimationGroup();
		void SetSingleFrameDuration(int frameDuration);
		void PlayAnimation();
		void DisableAnimation();
		void EnableAnimation();
		void SetAnimationFrame(uint FrameNumber) const;
		void AdjustViewportToCurrentDimensions();
		std::string GetName() override;
		void AddKeyFrame(KeyFrame keyFrame);

		ABCDRectangle Dimensions;
		std::vector<KeyFrame> KeyFrames;
		void SetAnimationFrameGroup(std::string group);
						
	private:
		std::string animationFrameGroup;
		unsigned long deltaTime;
		uint currentFrameNumber;
		uint startFrameNumber;
		float frameDurationMs;
		bool stopped = false;
		bool debug = false;

	};
}
