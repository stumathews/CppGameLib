#include "GameObject.h"
#include <memory>
#include <SDL_mixer.h>
#include "events/EventManager.h"
#include "events/ControllerMoveEvent.h"
#include "graphic/SDLGraphicsManager.h"


namespace gamelib
{
	using namespace std;

	GameObject::GameObject(const bool isVisible)
	{
		GameObject::LoadSettings();
		SetDefaults(isVisible, 0, 0);
	}

	GameObject::GameObject(const int x, const int y, const bool isVisible)
	{
		GameObject::LoadSettings();
		SetDefaults(isVisible, x, y);
	}

	GameObject::GameObject(const Coordinate<int> coordinate, const bool isVisible)
	{
		GameObject::LoadSettings();
		SetDefaults(isVisible, coordinate.GetX(), coordinate.GetY());
	}

	void GameObject::Draw(SDL_Renderer* renderer)
	{
	}

	SDL_Rect GameObject::CalculateBounds(const Coordinate<int> position, const int width, const int height)
	{
		return {position.GetX(), position.GetY(), width, height};
	}

	SDL_Rect GameObject::CalculateBounds(const Coordinate<int> position, const ABCDRectangle dimensions)
	{
		return {position.GetX(), position.GetY(), dimensions.GetWidth(), dimensions.GetHeight()};
	}

	void GameObject::LoadSettings()
	{
	}

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

	vector<shared_ptr<Event>> GameObject::HandleEvent(const std::shared_ptr<Event> event, unsigned long deltaMs)
	{
		return {};
	}

	int GameObject::GetSubscriberId() { return Id; }

	void GameObject::SetDefaults(const bool isVisible, const int x, const int y)
	{
		this->IsVisible = isVisible;

		Position.SetX(x);
		Position.SetY(y);

		// In this sensible?
		Bounds = {Position.GetX(), Position.GetY(), 0, 0};

		// Increase the Internal Object Id
		Id = lastGameObjectId++;
	}

	int GameObject::lastGameObjectId = 0;
}
