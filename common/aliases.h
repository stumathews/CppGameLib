#pragma once

#include "objects/GameObject.h"
#include <functional>
#include <memory>
#include <vector>
#include "events/Event.h"
#include <SDL.h>

namespace gamelib
{
	typedef unsigned int uint;
	typedef std::function<void(SDL_Renderer* renderer)> render_func;
	typedef std::vector<std::shared_ptr<gamelib::Event>> ListOfEvents;
}
