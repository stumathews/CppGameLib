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

	GameObject::GameObject(const float x, const float y, const bool isVisible)
	{
		GameObject::LoadSettings();
		SetDefaults(isVisible, x, y);
	}

	GameObject::GameObject(const Coordinate<float> coordinate, const bool isVisible)
	{
		GameObject::LoadSettings();
		SetDefaults(isVisible, coordinate.GetX(), coordinate.GetY());
	}

	void GameObject::Draw(SDL_Renderer* renderer) { }

	SDL_Rect GameObject::CalculateBounds(const Coordinate<float> position, const int width, const int height)
	{
		return {static_cast<int>(position.GetX()), static_cast<int>(position.GetY()), width, height};
	}

	SDL_Rect GameObject::CalculateBounds(const Coordinate<float> position, const AbcdRectangle &dimensions)
	{
		return {static_cast<int>(position.GetX()), static_cast<int>(position.GetY()), dimensions.GetWidth(), dimensions.GetHeight()};
	}

	void GameObject::LoadSettings()
	{
	}

	void GameObject::UpdateBounds(const unsigned int width, const unsigned int height)
	{
		Bounds = CalculateBounds(Position, static_cast<int>(width), static_cast<int>(height));
	}

	void GameObject::UpdateBounds(const AbcdRectangle& dimensions)
	{
		Bounds = CalculateBounds(Position, dimensions);
	}

	void GameObject::SetTag(const string& theTag) { this->tag = theTag; }

	string GameObject::GetName() { return Name; }
	string GameObject::GetSubscriberName() { return GetName(); }
	string GameObject::GetTag() const { return this->tag; }

	ListOfEvents GameObject::HandleEvent(const std::shared_ptr<Event>& event, const gamelib::DeltaTime* deltaTime)
	{
		return {};
	}

	int GameObject::GetSubscriberId() { return Id; }

	void GameObject::SetDefaults(const bool isVisible, const float x, const float y)
	{
		this->IsVisible = isVisible;

		Position.SetX(x);
		Position.SetY(y);

		// In this sensible?
		Bounds = {static_cast<int>(Position.GetX()), static_cast<int>(Position.GetY()), 0, 0};

		// Increase the Internal Object Id
		Id = lastGameObjectId++;
	}

	int GameObject::lastGameObjectId = 0;
}
