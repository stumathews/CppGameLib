#pragma once
#include <objects/DrawableGameObject.h>
//#include <objects/GameObject.h>
#include <common/aliases.h>
#include <graphic/KeyFrame.h>
//#include <asset/SpriteAsset.h>
#include <events/Event.h>

namespace gamelib
{
	class SpriteAsset;

	/// <summary>
    /// A static sprite may advance its sprite image manually or never
    /// </summary>
    class StaticSprite final : public DrawableGameObject
    {

    public:
        void Draw(SDL_Renderer* renderer) override;
        ListOfEvents HandleEvent(const std::shared_ptr<Event>& event, const unsigned long deltaMs) override;

        GameObjectType GetGameObjectType() override;
        void Update(const unsigned long deltaMs) override {}

        static std::shared_ptr<StaticSprite> Create(int x, int y, const std::shared_ptr<SpriteAsset>& spriteAsset);
        static std::shared_ptr<StaticSprite> Create(Coordinate<int> coordinate, const std::shared_ptr<SpriteAsset>& spriteAsset);
        
        void LoadSettings() override;
        void SetFrame(uint frameNumber);

        [[nodiscard]] uint GetNumKeyFrames() const;


        void AdvanceFrame();
    private:
        StaticSprite(Coordinate<int> position, const std::shared_ptr<SpriteAsset>& spriteAsset, bool isVisible);
        std::vector<KeyFrame> keyFrames;
        std::shared_ptr<SpriteAsset> asset;
        int frameNumber{};
    };
}
