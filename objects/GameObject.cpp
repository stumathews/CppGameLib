#include "GameObject.h"
#include <memory>
#include <SDL_mixer.h>
#include "audio/AudioAsset.h"
#include "common/constants.h"
#include "events/EventManager.h"
#include "events/PositionChangeEvent.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"
using namespace std;

namespace gamelib
{
	/// <summary>
	/// Game Object Ids - Static
	/// </summary>
	int GameObject::lastGameObjectId = 0;

	GameObject::GameObject(bool is_visible) : IEventSubscriber()
	{
		SetDefaults(is_visible, 0, 0);
		GameObject::LoadSettings();
	}

	GameObject::GameObject(const int x, const int y, bool is_visible ) : supportsMoveLogic(false)
	{
		SetDefaults(is_visible, x, y);
		GameObject::LoadSettings();
	}

	void GameObject::ChangeInternalPosition(const std::shared_ptr<Event> the_event)
	{
		const auto event = std::dynamic_pointer_cast<position_change_event>(the_event);
		if (event->direction == Direction::Up && supportsMoveLogic)
		{
			MoveUp();
		}
			
		if (event->direction == Direction::Down && supportsMoveLogic)
		{
			MoveDown();
		}
			
		if (event->direction == Direction::Left && supportsMoveLogic)
		{
			MoveLeft();
		}
			
		if (event->direction == Direction::Right && supportsMoveLogic)
		{
			MoveRight();
		}
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
	/// Get Graphic resource
	/// </summary>
	/// <returns></returns>
	shared_ptr<GraphicAsset> GameObject::GetGraphic() const
	{
		return graphic;
	}

	/// <summary>
	/// Draw Game Object
	/// </summary>
	/// <param name="renderer"></param>
	void GameObject::Draw(SDL_Renderer* renderer)
	{
		// Game Object usually does not draw itself... but decendents do.

		if (!isVisible)
		{
			return;
		}
		
		// We can draw the graphic resource if we have one however (this requires the decendents to call ensure they call GameObject::Draw() explicitly)
		if (HasGraphic())
		{
			DrawGraphic(renderer);
		}
	}

	/// <summary>
	/// Move Up
	/// </summary>
	void GameObject::MoveUp()
	{
		y -= moveInterval;
	}

	/// <summary>
	/// Move down
	/// </summary>
	void GameObject::MoveDown()
	{		
		y += moveInterval;
	}

	/// <summary>
	/// Move left
	/// </summary>
	void GameObject::MoveLeft()
	{
		x -= moveInterval;
	}

	/// <summary>
	/// Move right
	/// </summary>
	void GameObject::MoveRight()
	{
		x += moveInterval;
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
		moveInterval = SettingsManager::Get()->get_int("player", "move_interval");
	}

	/// <summary>
	/// Set defaults
	/// </summary>
	void GameObject::SetDefaults( bool is_visible, int x, int y)
	{
		 this->bounds = { x, y, 0 ,0};
		 supportsMoveLogic = true;
		 this->isVisible = is_visible;
		 isColorKeyEnabled = false;
		 this->x = x;
		 this->y = y;
		 is_traveling_left = false;
		 red = 0x00;
		 blue = 0xFF;
		 green = 0x00;
		 id = lastGameObjectId++;
	}

	/// <summary>
	/// Set color key
	/// </summary>
	void GameObject::SetColourKey(const Uint8 r, const Uint8 g, const Uint8 b)
	{
		color_key.r = r;
		color_key.g = g;
		color_key.b = b;
	}

	/// <summary>
	/// Add component to game object, eg. health, points etc
	/// </summary>
	void GameObject::AddComponent(const shared_ptr<component>& component)
	{
		components[component->get_name()] = component;
	}

	/// <summary>
	/// Find a component
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	shared_ptr<component> GameObject::FindComponent(string name)
	{
		return components[name];
	}

	/// <summary>
	/// Check if a component exists
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool GameObject::HasComponent(string name)
	{
		return components.find(name) != components.end();
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
	/// Draw graphic resource is one exists
	/// </summary>
	/// <param name="renderer"></param>
	void GameObject::DrawGraphic(SDL_Renderer* renderer) const
	{
		if (HasGraphic()) 
		{
			const auto graphic = GetGraphic();

			if (graphic->type == "graphic")
			{
				// Draw graphic at the game object's current location
				SDL_Rect drawLocation =
				{
					x,
					y,
					graphic->GetViewPort().w,
					graphic->GetViewPort().h
				};

				auto* const viewPort = graphic->IsAnimated() ? &graphic->GetViewPort() : nullptr;

				// Copy the texture (restricted by viewport) to the drawLocation on the screen
				SDL_RenderCopy(renderer, graphic->GetTexture(), viewPort, &drawLocation);
			}
			else
			{
				// Log error here
			}
		}
	}

	/// <summary>
	/// Check if game object has graphic resource
	/// </summary>
	/// <returns></returns>
	bool GameObject::HasGraphic() const
	{
		return graphic != nullptr;
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
	/// Set the graphic
	/// </summary>
	/// <param name="graphic"></param>
	void GameObject::SetGraphic(shared_ptr<GraphicAsset> graphic)
	{
		this->graphic = graphic;
	}
}