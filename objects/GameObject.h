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
	class GameObjectMover;

	

	/// <summary>
	/// Types of Game Objects
	/// </summary>
	enum class GameObjectType
	{
		Undefined,
		GameWorld,
		Room,
		Player,
		AnimatedSprite,
		StaticSprite,
		Pickup,
		DrawableFrameRate,
		Hotspot
	};

	class GameObject : public IEventSubscriber
	{
	public:

		// Create new Game Object 
		GameObject(bool is_visible);

		GameObject() = default;

		// Create new game object at specific coordinate
		GameObject(int x, int y, bool is_visible);
		GameObject(gamelib::coordinate<int> coordinate, bool IsVisible);
					
		/// <summary>
		/// Allow Game Object to subscribe to event system
		/// </summary>
		/// <param name="type"></param>
		/// <param name="event_admin"></param>
		void SubscribeToEvent(EventType type);

		/// <summary>
		/// Allow game object to raise an event
		/// </summary>
		/// <param name="the_event"></param>
		/// <param name="event_admin"></param>
		void RaiseEvent(const std::shared_ptr<Event>& the_event);

		/// <summary>
		/// Handle subscribed events
		/// </summary>
		/// <param name="event"></param>
		/// <returns></returns>
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event) override;

		/// <summary>
		/// Provide name to event system
		/// </summary>
		/// <returns></returns>
		std::string GetSubscriberName() override;

		/// <summary>
		/// Provide Id to event system
		/// </summary>
		/// <returns></returns>
		virtual int GetSubscriberId() override;
						
		/// <summary>
		/// Every Game Object can update itself itself diffirently
		/// </summary>
		void virtual Update(float deltaMs) = 0;

		/// <summary>
		/// All game objects can be drawn uniformly
		/// </summary>
		/// <param name="renderer"></param>
		void virtual Draw(SDL_Renderer* renderer) = 0;

		/// <summary>
		/// Every game Object needs to identify what type of game object it is
		/// </summary>
		/// <returns></returns>
		virtual GameObjectType GetGameObjectType() = 0;
				
		/// <summary>
		/// Every game object can identify itself in its own way
		/// </summary>
		/// <returns></returns>
		virtual std::string GetName();

		/// <summary>
		/// Every game object can load its own settings
		/// </summary>
		/// <param name="settings_admin"></param>
		virtual void LoadSettings();
	
		
		/// <summary>
		/// Arbitary tag for game object
		/// </summary>
		/// <returns></returns>
		std::string GetTag() const;

		/// <summary>
		/// Set Tag
		/// </summary>
		/// <param name="tag"></param>
		void SetTag(std::string tag);

		/// <summary>
		/// Each game object may be visible
		/// </summary>
		bool IsVisible;

		gamelib::coordinate<int> Position;

		/// <summary>
		/// Each game object has a bounds
		/// </summary>
		SDL_Rect Bounds;
		
		/// <summary>
		/// Each game object can be active
		/// </summary>
		bool IsActive = true;

		/// <summary>
		// Each game object has an ID
		/// </summary>
		int Id;	

		/// <summary>
		/// Inventory of components
		/// </summary>
		Inventory Components;

		std::map<std::string, std::string> stringProperties;

		SDL_Rect CalculateBounds(coordinate<int> Position, int _width, int height) { return { Position.GetX(), Position.GetY(), _width, height}; }
		
		void UpdateBounds(unsigned int inParentWidth, unsigned int inParentHeight);

	private:	

		// Game Object tag
		std::string tag;

		// Game Object Id counter
		static int lastGameObjectId;

		// Initialize game objects defaults
		void SetDefaults(bool IsVisible, int x, int y);
	};
}



