#pragma once

#include <functional>
#include <memory>
#include <vector>

struct SDL_Renderer; // Forward declaration for SDL_Renderer

namespace gamelib
{
	class Event;
	typedef unsigned int uint;
	typedef std::function<void(SDL_Renderer* renderer)> RenderFunc;
	typedef std::vector<std::shared_ptr<Event>> ListOfEvents;
}
