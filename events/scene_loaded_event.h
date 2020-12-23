#pragma once
#include "Event.h"
namespace gamelib
{
	class scene_loaded_event : public event
	{
	public:
		scene_loaded_event(int scene_id);
		std::string to_str() override;
		int scene_id_;
	};
}

