#pragma once

#include <map>
#include <SDL.h>
#include "Component.h"
#include "events/EventManager.h"
#include "Inventory.h"
#include <string>
#include "util/Tuple.h"

namespace gamelib
{
	enum class GameObjectType
	{
		Undefined,
		GameDefined, // Game Object is custom made and has no special handling in the library
		AnimatedSprite,
		StaticSprite,
		Pickup,
		DrawableFrameRate,
		Hotspot
	};

	class GameObject : public IEventSubscriber
	{
	public:

		GameObject(bool is_visible);
		GameObject() = default;
		GameObject(int x, int y, bool is_visible);
		GameObject(std::string name, std::string type, gamelib::coordinate<int> coordinate, bool IsVisible);
							
		void SubscribeToEvent(EventType type);
		void RaiseEvent(const std::shared_ptr<Event>& the_event);
		void SetTag(std::string tag);
		void UpdateBounds(unsigned int inParentWidth, unsigned int inParentHeight);
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs) override;
		std::string GetSubscriberName() override;
		std::string GetTag() const;
		virtual int GetSubscriberId() override;
		virtual GameObjectType GetGameObjectType() = 0;
		virtual std::string GetName();
		virtual void LoadSettings();
		virtual void Update(float deltaMs) = 0;
		virtual void Draw(SDL_Renderer* renderer) = 0;
		SDL_Rect CalculateBounds(coordinate<int> Position, int _width, int height) { return { Position.GetX(), Position.GetY(), _width, height }; }

		bool IsVisible;
		bool IsActive = true;
		SDL_Rect Bounds;
		int Id;	
		gamelib::coordinate<int> Position;
		Inventory Components;
		std::map<std::string, std::string> stringProperties;
		std::string Name;
		std::string Type;

	private:	

		// Game Object tag
		std::string tag;

		// Game Object Id counter
		static int lastGameObjectId;

		// Initialize game objects defaults
		void SetDefaults(bool IsVisible, int x, int y);
	};
}



