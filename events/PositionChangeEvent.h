#pragma once
#include "Event.h"

namespace gamelib
{
	enum class Direction {Up, Down, Left, Right};
	
	class position_change_event : public event
	{
	public:
		explicit position_change_event(Direction dir);
		Direction direction;
		std::string to_str() override;
	};
}


