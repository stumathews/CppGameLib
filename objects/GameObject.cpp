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
	GameObject::GameObject(bool isVisible) : IEventSubscriber()
	{
		SetDefaults(isVisible, 0, 0);
		GameObject::LoadSettings();
	}

	GameObject::GameObject(const int x, const int y, bool isVisible ) : supportsMoveLogic(false)
	{
		SetDefaults(isVisible, x, y);
		GameObject::LoadSettings();
	}

	void GameObject::ChangeInternalPosition(const std::shared_ptr<Event> event)
	{
		const auto positionChangeEvent = std::dynamic_pointer_cast<PositionChangeEvent>(event);
		
		if (positionChangeEvent->direction == Direction::Up && supportsMoveLogic)
		{
			MoveUp();
		}
			
		if (positionChangeEvent->direction == Direction::Down && supportsMoveLogic)
		{
			MoveDown();
		}
			
		if (positionChangeEvent->direction == Direction::Left && supportsMoveLogic)
		{
			MoveLeft();
		}
			
		if (positionChangeEvent->direction == Direction::Right && supportsMoveLogic)
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
	void GameObject::SetDefaults(bool isVisible, int x, int y)
	{
		this->isVisible = isVisible;
		this->x = x;
		this->y = y;

		// In this sensible?
		bounds = { x, y, 0 , 0 };

		// Should all game Objects support move 
		supportsMoveLogic = true;

		// Should all game Objects support ColourKey
		isColorKeyEnabled = false;
		
		red = 0x00;
		blue = 0xFF;
		green = 0x00;

		// Increase the Internal Object Id
		id = lastGameObjectId++;
	}

	/// <summary>
	/// Set color key
	/// </summary>
	void GameObject::SetColourKey(const Uint8 r, const Uint8 g, const Uint8 b)
	{
		colourKey.r = r;
		colourKey.g = g;
		colourKey.b = b;
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
					x, y,
					graphic->GetViewPort().w, 
					graphic->GetViewPort().h
				};

				// Copy the texture (restricted by viewport) to the drawLocation on the screen
				SDL_RenderCopy(renderer, graphic->GetTexture(), &graphic->GetViewPort(), &drawLocation);
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

	/// <summary>
	/// Game Object Ids - Static
	/// </summary>
	int GameObject::lastGameObjectId = 0;

}