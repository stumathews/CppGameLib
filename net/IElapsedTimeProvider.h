#pragma once

namespace gamelib
{
	class IElapsedTimeProvider  // NOLINT(cppcoreguidelines-special-member-functions)
	{
	public:
		virtual ~IElapsedTimeProvider() = default;
		virtual int GetElapsedTime() = 0;
	};
}