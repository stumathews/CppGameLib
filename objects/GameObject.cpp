#include "GameObject.h"
#include <memory>
#include <SDL_mixer.h>
#include "events/EventManager.h"
#include "events/ControllerMoveEvent.h"
#include "graphic/SDLGraphicsManager.h"


namespace gamelib
{	
	using namespace std;

	GameObject::GameObject(const bool isVisible) { GameObject::LoadSettings(); SetDefaults(isVisible, 0, 0); }
	GameObject::GameObject(const int x, const int y, const bool isVisible ) { GameObject::LoadSettings(); SetDefaults(isVisible, x, y); }
	GameObject::GameObject(const Coordinate<int> coordinate, const bool isVisible) { GameObject::LoadSettings(); SetDefaults(isVisible, coordinate.GetX(), coordinate.GetY()); }
	
	void GameObject::SubscribeToEvent(const EventId& eventId)
	{
		EventManager::Get()->SubscribeToEvent(eventId, this);
		EventSubscriptions.push_back(eventId);
	}

	void GameObject::UnsubscribeSubscribeToEvent(const EventId& eventId)
	{		
		EventManager::Get()->Unsubscribe(eventId.PrimaryId);
		EventSubscriptions.remove(eventId);
	}

	bool GameObject::SubscribesTo(const EventId& eventId)
	{
		return std::find(begin(EventSubscriptions), end(EventSubscriptions), eventId) != end(EventSubscriptions);
	}

	void GameObject::RaiseEvent(const shared_ptr<Event>& theEvent) { EventManager::Get()->RaiseEvent(theEvent, this); }
	void GameObject::Draw(SDL_Renderer* renderer) { }	
	void GameObject::LoadSettings() {}
	void GameObject::UpdateBounds(const unsigned int width, const unsigned int height)
	{
		Bounds = CalculateBounds(Position, static_cast<int>(width), static_cast<int>(height));
	}
	void GameObject::UpdateBounds(const ABCDRectangle dimensions)
	{
		Bounds = CalculateBounds(Position, dimensions);
	}
	void GameObject::SetTag(const string& tag) { this->tag = tag; }

	string GameObject::GetName() { return Name; }
	string GameObject::GetSubscriberName() { return GetName(); }
	string GameObject::GetTag() const { return this->tag; }

	vector<shared_ptr<Event>> GameObject::HandleEvent(const std::shared_ptr<Event> event, unsigned long deltaMs) { return {}; }

	int GameObject::GetSubscriberId() { return Id; }

	void GameObject::SetDefaults(const bool isVisible, const int x, const int y)
	{
		this->IsVisible = isVisible;
		
		Position.SetX(x);
		Position.SetY(y);

		// In this sensible?
		Bounds = { Position.GetX(), Position.GetY(), 0 , 0 };
		
		// Increase the Internal Object Id
		Id = lastGameObjectId++;
	}	

	int GameObject::lastGameObjectId = 0;
}