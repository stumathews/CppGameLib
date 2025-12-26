#pragma once
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
		
		AnimatedSprite(const std::string& name, const std::string& type, Coordinate<int> position, float frameDuration,
		               bool isVisible,
		               const std::shared_ptr<SpriteAsset>& spriteAsset);
		AnimatedSprite() = default;

		static std::shared_ptr<AnimatedSprite> Create(Coordinate<int> position, const std::shared_ptr<SpriteAsset>& asset);
		static void Initialize();

		GameObjectType GetGameObjectType() override { return GameObjectType::animated_sprite; }
		void Draw(SDL_Renderer* renderer) override;	
		void LoadSettings() override;
		void MoveSprite(int x, int y);
		void MoveSprite(Coordinate<int> position);
		void Update(unsigned long deltaMs) override;
		void Update(unsigned long deltaMs, const std::string& animationGroup);
		void AdvanceCurrentFrameNumber();
		void SkipUnsupportedAnimationGroupFrames();
		[[nodiscard]] bool IsCurrentFrameInAnimationGroup() const;
		void SetSingleFrameDuration(int frameDuration);
		void PlayAnimation();
		void DisableAnimation();
		void EnableAnimation();
		std::string GetName() override;
		void AddKeyFrame(const KeyFrame& keyFrame);

		AbcdRectangle Dimensions{};
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
