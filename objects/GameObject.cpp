#include "GameObject.h"
#include <memory>
#include <SDL_mixer.h>
#include "audio/AudioAsset.h"
#include "common/constants.h"
#include "events/EventManager.h"
#include "events/ControllerMoveEvent.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"
#include "GameObjectMover.h"


namespace gamelib
{	
	using namespace std;

	GameObject::GameObject(bool IsVisible) { GameObject::LoadSettings(); SetDefaults(IsVisible, 0, 0); }
	GameObject::GameObject(const int x, const int y, bool IsVisible ) { GameObject::LoadSettings(); SetDefaults(IsVisible, x, y); }
	GameObject::GameObject(gamelib::coordinate<int> coordinate, bool IsVisible) { GameObject::LoadSettings(); SetDefaults(IsVisible, coordinate.GetX(), coordinate.GetY()); }
	
	void GameObject::SubscribeToEvent(EventType type) { EventManager::Get()->SubscribeToEvent(type, this); }
	void GameObject::RaiseEvent(const shared_ptr<Event>& the_event) { EventManager::Get()->RaiseEvent(the_event, this); }
	void GameObject::Draw(SDL_Renderer* renderer) { }	
	void GameObject::LoadSettings() {}
	void GameObject::UpdateBounds(unsigned int _width, unsigned int height) { Bounds = CalculateBounds(Position, _width, height); }
	void GameObject::SetTag(const string newTag) { tag = newTag; }

	string GameObject::GetName() { return "game_object"; }
	string GameObject::GetSubscriberName() { return GetName(); }
	string GameObject::GetTag() const { return this->tag; }

	vector<shared_ptr<Event>> GameObject::HandleEvent(const std::shared_ptr<Event> the_event, unsigned long deltaMs) { return vector<shared_ptr<Event>>(); }

	int GameObject::GetSubscriberId() { return Id; }

	void GameObject::SetDefaults(bool newIsVisible, int x, int y)
	{
		IsVisible = newIsVisible;
		
		Position.SetX(x);
		Position.SetY(y);

		// In this sensible?
		Bounds = { Position.GetX(), Position.GetY(), 0 , 0 };
		
		// Increase the Internal Object Id
		Id = lastGameObjectId++;
	}	

	int GameObject::lastGameObjectId = 0;
}