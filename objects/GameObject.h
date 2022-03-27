#pragma once

#include <map>
#include <SDL.h>
#include "Component.h"
#include <graphic/GraphicAsset.h>
#include "events/EventManager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>

namespace gamelib
{
	/// <summary>
	/// Types of Game Objects
	/// </summary>
	enum class object_type
	{
		Undefined,
		GameWorld,
		Room,
		Player,
		AnimatedSprite,
		Pickup,
	};

	class GameObject : public IEventSubscriber
	{
	public:

		// Create new Game Object 
		GameObject(bool is_visible);

		// Create new game object at specific coordinate
		GameObject(int x, int y, bool is_visible);
					
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
		std::vector<std::shared_ptr<Event>> HandleEvent( std::shared_ptr<Event> event) override;

		/// <summary>
		/// Provide name to event system
		/// </summary>
		/// <returns></returns>
		std::string GetSubscriberName() override;

		/// <summary>
		/// Provide Id to event system
		/// </summary>
		/// <returns></returns>
		virtual int GetSubscriberId() override { return id; }
		
		/// <summary>
		/// Get the Game Object's graphic if there is one associated with the game object
		/// </summary>
		/// <returns>GraphicResource</returns>
		std::shared_ptr<GraphicAsset> GetGraphic() const;

		/// <summary>
		/// Set the Game Object's graphic
		/// </summary>
		/// <param name="graphic"></param>
		void SetGraphic(std::shared_ptr<GraphicAsset> graphic);
		bool HasGraphic() const;
		
		/// <summary>
		/// Every Game Object can update itself itself diffirently
		/// </summary>
		void virtual Update() = 0;

		/// <summary>
		/// All game objects can be drawn uniformly
		/// </summary>
		/// <param name="renderer"></param>
		void virtual Draw(SDL_Renderer* renderer);

		/// <summary>
		/// Every game Object needs to identify what type of game object it is
		/// </summary>
		/// <returns></returns>
		virtual object_type GetGameObjectType() = 0;

		// Every Game Object can move in its own way
		virtual void MoveUp();
		virtual void MoveDown();
		virtual void MoveLeft();
		virtual void MoveRight();

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
		/// Every game Object can change it own internal position
		/// </summary>
		/// <param name="the_event"></param>
		virtual void ChangeInternalPosition(std::shared_ptr<Event> the_event);
		
		void SetColourKey(Uint8 r, Uint8 g, Uint8 b);

		/* Inventory System: game objects can have a list of components */
		
		/// <summary>
		/// Add component to the game object (inventory)
		/// </summary>
		/// <param name="component"></param>
		void AddComponent(const std::shared_ptr<component>& component);
		std::shared_ptr<component> FindComponent(std::string name);
		bool HasComponent(std::string name);

		/// <summary>
		/// Arbitary tag for game object
		/// </summary>
		/// <returns></returns>
		std::string GetTag() const;
		void SetTag(std::string tag);

		/// <summary>
		/// Each game object may support moving
		/// </summary>
		bool supportsMoveLogic;

		/// <summary>
		/// Each game object may be visible
		/// </summary>
		bool isVisible;

		/// <summary>
		/// Each object may have a colour key enabled
		/// </summary>
		bool isColorKeyEnabled;

		/// <summary>
		/// Each game object has a x-orordinate
		/// </summary>
		int x;

		/// <summary>
		/// Each game object has a y-orordinate
		/// </summary>
		int y;

		/// <summary>
		/// Each game object has a move internal
		/// </summary>
		int moveInterval;	

		/// <summary>
		/// Each game object has a bounds
		/// </summary>
		SDL_Rect bounds;
		
		/// <summary>
		/// Each game object can be active
		/// </summary>
		bool isActive = true;

		/// <summary>
		// Each game object has an ID
		/// </summary>
		int id;	

	private:	

		// Game Object tag
		std::string tag;
		bool is_traveling_left;

		// Game Object colour
		int red, blue, green;
		SDL_Color color_key = {};

		// Game Object Graphic
		std::shared_ptr<GraphicAsset> graphic; // can be shared by other actors
		
		// list of components
		std::map<std::string, std::shared_ptr<component>> components;
		
		// Underlying asset
		std::shared_ptr<Asset> assetInfo;

		// Game Object Id counter
		static int lastGameObjectId;

		// Initialize game objects defaults
		void SetDefaults(bool isVisible, int x, int y);

		
		void DrawGraphic(SDL_Renderer* renderer) const;


	};
}



