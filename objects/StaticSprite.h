#pragma once
#include <objects/DrawableGameObject.h>
#include <objects/GameObject.h>
#include <common/aliases.h>
#include <KeyFrame.h>
#include <SpriteAsset.h>
#include <Timer.h>
#include <events/Event.h>
#include <events/IEventSubscriber.h>

namespace gamelib
{
    /// <summary>
    /// A static sprite may advance its sprite image manually or never
    /// </summary>
    class StaticSprite : public gamelib::DrawableGameObject
    {

    public:
        // Inherited via DrawableGameObject
        virtual void Draw(SDL_Renderer* renderer) override;
        std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;

        virtual gamelib::GameObjectType GetGameObjectType() override;
        virtual void Update(float deltaMs) override {};

        /// <summary>
        /// Create sprite
        /// </summary>
        static std::shared_ptr<StaticSprite> Create(int x, int y, std::shared_ptr<gamelib::SpriteAsset> spriteAsset);

        /// <summary>
        /// Create sprite
        /// </summary>
        /// <returns></returns>
        static std::shared_ptr<StaticSprite> Create(gamelib::coordinate<int> coordinate, std::shared_ptr<gamelib::SpriteAsset> spriteAsset);

        /// <summary>
        /// Ordered set of key frames
        /// </summary>
        void LoadSettings();
        void SetFrame(gamelib::uint FrameNumber);
        gamelib::uint GetNumKeyFrames()
        {
            return KeyFrames.size();
        }


        void AdvanceFrame();
    private:
        // Create a static sprite
        StaticSprite(gamelib::coordinate<int> position, std::shared_ptr<gamelib::SpriteAsset> spriteAsset, bool IsVisible);
        std::vector<gamelib::KeyFrame> KeyFrames;
        std::shared_ptr<gamelib::SpriteAsset> asset;
        int currentFrame;
    };
}
