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

	GameObject::GameObject(bool IsVisible)
	{
		GameObject::LoadSettings();
		SetDefaults(IsVisible, 0, 0);
	}

	GameObject::GameObject(const int x, const int y, bool IsVisible )
	{
		GameObject::LoadSettings();
		SetDefaults(IsVisible, x, y);		
	}

	GameObject::GameObject(gamelib::coordinate<int> coordinate, bool IsVisible)
	{
		GameObject::LoadSettings();
		SetDefaults(IsVisible, coordinate.GetX(), coordinate.GetY());
	}

	
	/// <summary>
	/// Handle any Game Object events
	/// </summary>
	vector<shared_ptr<Event>> GameObject::HandleEvent(const std::shared_ptr<Event> the_event)
	{
		// We dont handle any events
		return vector<shared_ptr<Event>>();
	}

	/// <summary>
	/// All Game object so directly Subscribe to event on underlying event manager
	/// </summary>
	void GameObject::SubscribeToEvent(EventType type)
	{
		EventManager::Get()->SubscribeToEvent(type, this);
	}

	/// <summary>
	/// Raise event on underlying event manager
	/// </summary>
	void GameObject::RaiseEvent(const shared_ptr<Event>& the_event)
	{
		EventManager::Get()->RaiseEvent(the_event, this);
	}

	/// <summary>
	/// Draw Game Object
	/// </summary>
	/// <param name="renderer"></param>
	void GameObject::Draw(SDL_Renderer* renderer)
	{
		// Game Object usually does not draw itself... but decendents do.
	}
	
	/// <summary>
	/// Get Name
	/// </summary>
	/// <returns></returns>
	string GameObject::GetName()
	{
		return "game_object";
	}

	/// <summary>
	/// Load Game Object settings
	/// </summary>
	/// <param name="settings_admin"></param>
	void GameObject::LoadSettings()
	{
		
	}

	/// <summary>
	/// Set defaults
	/// </summary>
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

	/// <summary>
	/// Set Tag
	/// </summary>
	/// <param name="tag"></param>
	void GameObject::SetTag(const string newTag)
	{
		tag = newTag;
	}

	string GameObject::GetSubscriberName()
	{
		return "game_object";
	}

	/// <summary>
	/// Provide Id to event system
	/// </summary>
	/// <returns>SubscriberId</returns>

	int GameObject::GetSubscriberId() 
	{
		return Id; 
	}

	/// <summary>
	/// Get Tag
	/// </summary>
	/// <returns>Arbitrary tag</returns>
	string GameObject::GetTag() const
	{
		return this->tag;
	}

	/// <summary>
	/// Game Object Ids - Static
	/// </summary>
	int GameObject::lastGameObjectId = 0;

}