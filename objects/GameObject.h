#pragma once

#include <map>
#include <SDL.h>
#include "events/EventManager.h"
#include "character/Inventory.h"
#include <string>
#include "geometry/ABCDRectangle.h"
#include "util/Tuple.h"
#include <list>

namespace gamelib
{
	typedef std::vector<std::shared_ptr<Event>> ListOfEvents;
	enum class GameObjectType
	{
		Undefined,
		GameDefined,
		// Game Object is custom made and has no special handling in the library
		AnimatedSprite,
		StaticSprite,
		Pickup,
		DrawableFrameRate,
		Hotspot
	};

	class GameObject : public EventSubscriber
	{
	public:
		explicit GameObject(bool isVisible);
		GameObject() = default;
		GameObject(int x, int y, bool isVisible);
		GameObject(Coordinate<int> coordinate, bool isVisible);
		void SetTag(const std::string& tag);
		void UpdateBounds(unsigned int width, unsigned int height);
		void UpdateBounds(AbcdRectangle dimensions);
		ListOfEvents HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs) override;
		std::string GetSubscriberName() override;
		[[nodiscard]] std::string GetTag() const;
		virtual int GetSubscriberId() override;
		virtual GameObjectType GetGameObjectType() = 0;
		virtual std::string GetName();
		virtual void LoadSettings();
		virtual void Update(const unsigned long deltaMs) = 0;
		virtual void Draw(SDL_Renderer* renderer) = 0;
		static SDL_Rect CalculateBounds(const Coordinate<int> position, const int width, const int height);
		static SDL_Rect CalculateBounds(const Coordinate<int> position, const AbcdRectangle dimensions);

		bool IsVisible{};
		bool IsActive = true;
		SDL_Rect Bounds{};
		int Id{};
		Coordinate<int> Position;
		std::map<std::string, std::string> StringProperties;
		std::map<std::string, int> IntProperties;
		std::string Name;
		std::string Type;

	private:
		// Game Object tag
		std::string tag;

		// Game Object Id counter
		static int lastGameObjectId;

		// Initialize game objects defaults
		void SetDefaults(bool isVisible, int x, int y);
	};
}
