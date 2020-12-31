#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "events/Event.h"

namespace gamelib
{
	class game_object;
	typedef unsigned int uint;
	typedef std::vector<std::shared_ptr<event>> events;
	typedef std::function<void(SDL_Renderer* renderer)> render_func;
	typedef std::vector<std::shared_ptr<game_object>> game_objects;
}
