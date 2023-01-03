#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "events/Event.h"
#include <SDL.h>

namespace gamelib
{
	typedef unsigned int uint;
	typedef std::function<void(SDL_Renderer* renderer)> RenderFunc;
	typedef std::vector<std::shared_ptr<Event>> ListOfEvents;
}
