#pragma once
#include "Event.h"

namespace gamelib
{
	/// <summary>
	/// An event that informs the receiver to do a logic update, without actually coupling to the receiver
	/// </summary>
	class LogicUpdateEvent final : public Event
	{
	public:
		/// <summary>
		/// Create a Logic Update Event
		/// </summary>
		LogicUpdateEvent();

		/// <summary>
		/// All events should have a string version
		/// </summary>
		/// <returns></returns>
		std::string ToString() override;
	};
}
