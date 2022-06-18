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

	GameObject::GameObject(bool isVisible)
	{
		GameObject::LoadSettings();
		SetDefaults(isVisible, 0, 0);
	}

	GameObject::GameObject(const int x, const int y, bool isVisible )
	{
		GameObject::LoadSettings();
		SetDefaults(isVisible, x, y);		
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
	void GameObject::SetDefaults(bool isVisible, int x, int y)
	{
		this->isVisible = isVisible;
		/*this->x = x;
		this->y = y;*/
		Position.SetX(x);
		Position.SetY(y);

		// In this sensible?
		Bounds = { Position.GetX(), Position.GetY(), 0 , 0 };
		
		// mover supports move operations
		//mover = shared_ptr<GameObjectMover>(new GameObjectMover(this, SettingsManager::Get()->get_int("player", "move_interval")));

		// Increase the Internal Object Id
		Id = lastGameObjectId++;
	}	

	/// <summary>
	/// Set Tag
	/// </summary>
	/// <param name="tag"></param>
	void GameObject::SetTag(const string tag)
	{
		this->tag = tag;
	}

	string GameObject::GetSubscriberName()
	{
		return "game_object";
	}

	/// <summary>
	/// Provide Id to event system
	/// </summary>
	/// <returns></returns>

	int GameObject::GetSubscriberId() 
	{
		return Id; 
	}

	/// <summary>
	/// Get Tag
	/// </summary>
	/// <returns></returns>
	string GameObject::GetTag() const
	{
		return this->tag;
	}

	/// <summary>
	/// Game Object Ids - Static
	/// </summary>
	int GameObject::lastGameObjectId = 0;

}