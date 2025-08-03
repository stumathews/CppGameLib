#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <map>

//#include "character/Inventory.h"
#include <memory>
#include <string>
#include <vector>

#include "events/EventSubscriber.h"
#include "geometry/ABCDRectangle.h"
#include "geometry/Coordinate.h"
#include "objects/GameObjectType.h"

namespace gamelib
{
	class Event;
	typedef std::vector<std::shared_ptr<Event>> ListOfEvents;


	class GameObject : public EventSubscriber
	{
	public:
		explicit GameObject(bool isVisible);
		GameObject() = default;
		GameObject(int x, int y, bool isVisible);
		GameObject(Coordinate<int> coordinate, bool isVisible);

		// Get/Set tag:
		void SetTag(const std::string& tag);
		[[nodiscard]] std::string GetTag() const;

		// Bounding boxes:
		void UpdateBounds(unsigned int width, unsigned int height);
		void UpdateBounds(const AbcdRectangle& dimensions);
		static SDL_Rect CalculateBounds(const Coordinate<int> position, const int width, const int height);
		static SDL_Rect CalculateBounds(const Coordinate<int> position, const AbcdRectangle dimensions);

		// Event functions:
		ListOfEvents HandleEvent(const std::shared_ptr<Event>& event, const unsigned long deltaMs) override;
		std::string GetSubscriberName() override;
		virtual int GetSubscriberId() override;

		// Game object identification:
		virtual GameObjectType GetGameObjectType() = 0;
		virtual std::string GetName();

		// Settings
		virtual void LoadSettings();

		// All game objects must perform Logic
		virtual void Update(const unsigned long deltaMs) = 0;

		// All game objects must draw
		virtual void Draw(SDL_Renderer* renderer) = 0;

		// Game objects can contain string and number properties

		std::map<std::string, std::string> StringProperties;
		std::map<std::string, int> IntProperties;

		std::string Name;
		std::string Type;
		int Id{};

		bool IsVisible{};
		bool IsActive = true;

		Coordinate<int> Position;

		SDL_Rect Bounds{};

	private:
		// Game Object tag
		std::string tag;

		// Game Object Id counter
		static int lastGameObjectId;

		// Initialize game objects defaults
		void SetDefaults(bool isVisible, int x, int y);
	};
}

#endif