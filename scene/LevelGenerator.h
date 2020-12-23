#pragma once
#include <vector>
#include <memory>
#include "Square.h"

namespace gamelib
{
	class level_generator
	{
	public:
		enum room_side {top_side = 1, right_side = 2, bottom_side = 3, left_side = 4};
		static std::vector<std::shared_ptr<square>> generate_level(std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings_admin);	
	};
}

