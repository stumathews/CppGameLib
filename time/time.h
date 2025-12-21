#pragma once

#include <chrono>
#include <ctime>

namespace gamelib
{

	uint64_t GetTimeMs();

	std::tm localtime_safe(std::time_t t);
}
