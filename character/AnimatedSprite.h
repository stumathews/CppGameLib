#pragma once
#include <gtest/gtest-assertion-result.h>

#include "character/Direction.h"
#include "common/TypeAliases.h"
#include "objects/GameObject.h"
#include "graphic/KeyFrame.h"
#include "geometry/ABCDRectangle.h"
#include "objects/DrawableGameObject.h"

struct SDL_render;

namespace gamelib
{
	class SpriteAsset;

	/// <summary>
	/// Sprite sheet based sprite
	/// </summary>
	class AnimatedSprite final : public DrawableGameObject
	{	
	public:
		
		AnimatedSprite(const std::string& name, const std::string& type, Coordinate<float> position, float frameDuration,
		               bool isVisible,
		               const std::shared_ptr<SpriteAsset>& spriteAsset);
		AnimatedSprite() = default;

		static std::shared_ptr<AnimatedSprite> Create(Coordinate<float> position, const std::shared_ptr<SpriteAsset>& asset);
		static void Initialize();

		GameObjectType GetGameObjectType() override { return GameObjectType::animated_sprite; }
		void Draw(SDL_Renderer* renderer) override;	
		void LoadSettings() override;
		void MoveSprite(float x, float y);
		void MoveSprite(Coordinate<float> position);
		void Update(const gamelib::DeltaTime* deltaTime) override;
		void Update(const gamelib::DeltaTime* deltaTime, const std::string& animationGroup);
		void AdvanceCurrentFrameNumber();
		void SkipUnsupportedAnimationGroupFrames();
		[[nodiscard]] bool IsCurrentFrameInAnimationGroup() const;
		void SetSingleFrameDuration(int frameDuration);
		void PlayAnimation();
		void DisableAnimation();
		void EnableAnimation();
		std::string GetName() override;
		void AddKeyFrame(const KeyFrame& keyFrame);

		bool IsStopped();

		AbcdRectangle Dimensions{};
		std::vector<KeyFrame> KeyFrames;


		void SetAnimationFrameGroup(const std::string& group);
		static std::string GetStdDirectionAnimationFrameGroup(Direction facingDirection);
		std::shared_ptr<SpriteAsset> Asset;

		int GetCurrentKeyFrameIndex() const;
	private:

		std::string animationFrameGroup;
		unsigned long deltaTime{};
		uint currentFrameNumber{};
		uint startFrameNumber{};
		float frameDurationMs{};
		unsigned long durationSinceLastFrameMs {};
		bool stopped = false;
		bool debug = false;

	};
}
