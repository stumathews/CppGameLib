#pragma once
#include <objects/DrawableGameObject.h>
#include <objects/GameObject.h>
#include <common/aliases.h>
#include <KeyFrame.h>
#include <SpriteAsset.h>
#include <Timer.h>
#include <events/Event.h>

namespace gamelib
{
    /// <summary>
    /// A static sprite may advance its sprite image manually or never
    /// </summary>
    class StaticSprite final : public DrawableGameObject
    {

    public:
        // Inherited via DrawableGameObject
        void Draw(SDL_Renderer* renderer) override;
        std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs) override;

        GameObjectType GetGameObjectType() override;
        void Update(const unsigned long deltaMs) override {}

        /// <summary>
        /// Create sprite
        /// </summary>
        static std::shared_ptr<StaticSprite> Create(int x, int y, const std::shared_ptr<SpriteAsset>&
                                                    spriteAsset);

        /// <summary>
        /// Create sprite
        /// </summary>
        /// <returns></returns>
        static std::shared_ptr<StaticSprite> Create(Coordinate<int> coordinate, const std::shared_ptr<SpriteAsset>
                                                    & spriteAsset);

        /// <summary>
        /// Ordered set of key frames
        /// </summary>
        void LoadSettings() override;
        void SetFrame(uint frameNumber) const;

        [[nodiscard]] uint GetNumKeyFrames() const
        {
            return keyFrames.size();
        }


        void AdvanceFrame();
    private:
        // Create a static sprite
        StaticSprite(Coordinate<int> position, const std::shared_ptr<SpriteAsset>& spriteAsset, bool isVisible);
        std::vector<KeyFrame> keyFrames;
        std::shared_ptr<SpriteAsset> asset;
        int currentFrame{};
    };
}
