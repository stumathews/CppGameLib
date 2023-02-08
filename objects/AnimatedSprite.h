#pragma once
#include "Direction.h"
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
		AnimatedSprite(const std::string& name, const std::string& type, Coordinate<int> position, float frameDuration, bool isVisible, std::shared_ptr<SpriteAsset> spriteAsset);
		AnimatedSprite() = default;
		static std::shared_ptr<AnimatedSprite> Create(Coordinate<int> position, const std::shared_ptr<SpriteAsset>& asset);
		static void Initialize(const std::shared_ptr<AnimatedSprite>& animatedSprite);
		/// <summary>
		/// Set game object type
		/// </summary>
		GameObjectType GetGameObjectType() override { return GameObjectType::AnimatedSprite; }
		void Draw(SDL_Renderer* renderer) override;	
		void LoadSettings() override;
		void MoveSprite(int x, int y);
		void MoveSprite(Coordinate<int> position);
		void Update(const unsigned long deltaMs) override;
		void Update(unsigned long deltaMs, const std::string& animationGroup);
		void AdvanceCurrentFrameNumber();
		void SkipUnsupportedAnimationGroupFrames();
		[[nodiscard]] bool IsCurrentFrameInAnimationGroup() const;
		void SetSingleFrameDuration(int frameDuration);
		void PlayAnimation();
		void DisableAnimation();
		void EnableAnimation();
		void SetAnimationFrame(uint FrameNumber) const;
		void AdjustViewportToCurrentDimensions() const;
		std::string GetName() override;
		void AddKeyFrame(const KeyFrame& keyFrame);

		ABCDRectangle Dimensions{};
		std::vector<KeyFrame> KeyFrames;
		void SetAnimationFrameGroup(const std::string& group);
		static std::string GetStdDirectionAnimationFrameGroup(Direction facingDirection);
		std::shared_ptr<SpriteAsset> Asset;
						
	private:
		std::string animationFrameGroup;
		unsigned long deltaTime{};
		uint currentFrameNumber{};
		uint startFrameNumber{};
		float frameDurationMs{};
		bool stopped = false;
		bool debug = false;

	};
}
