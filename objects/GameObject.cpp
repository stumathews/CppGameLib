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
	GameObject::GameObject(const int x, const int y, const bool IsVisible ) { GameObject::LoadSettings(); SetDefaults(IsVisible, x, y); }
	GameObject::GameObject(std::string name, std::string type, const gamelib::Coordinate<int> coordinate, const bool isVisible) { GameObject::LoadSettings(); SetDefaults(isVisible, coordinate.GetX(), coordinate.GetY()); }
	
	void GameObject::SubscribeToEvent(const EventType type) { EventManager::Get()->SubscribeToEvent(type, this); }
	void GameObject::RaiseEvent(const shared_ptr<Event>& theEvent) { EventManager::Get()->RaiseEvent(theEvent, this); }
	void GameObject::Draw(SDL_Renderer* renderer) { }	
	void GameObject::LoadSettings() {}
	void GameObject::UpdateBounds(const unsigned int inParentWidth, const unsigned int inParentHeight)
	{
		Bounds = CalculateBounds(Position, inParentWidth, inParentHeight);
	}
	void GameObject::SetTag(const string& newTag) { tag = newTag; }

	string GameObject::GetName() { return Name; }
	string GameObject::GetSubscriberName() { return GetName(); }
	string GameObject::GetTag() const { return this->tag; }

	vector<shared_ptr<Event>> GameObject::HandleEvent(const std::shared_ptr<Event> theEvent, unsigned long deltaMs) { return {}; }

	int GameObject::GetSubscriberId() { return Id; }

	void GameObject::SetDefaults(const bool isVisible, const int x, const int y)
	{
		IsVisible = isVisible;
		
		Position.SetX(x);
		Position.SetY(y);

		// In this sensible?
		Bounds = { Position.GetX(), Position.GetY(), 0 , 0 };
		
		// Increase the Internal Object Id
		Id = lastGameObjectId++;
	}	

	int GameObject::lastGameObjectId = 0;
}