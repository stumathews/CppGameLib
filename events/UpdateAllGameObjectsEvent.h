#pragma once
#ifndef UPDATEALLGAMEOBJECTEVENT_H
#define UPDATEALLGAMEOBJECTEVENT_H

#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	const static EventId UpdateAllGameObjectsEventTypeEventId(UpdateAllGameObjectsEventType, "UpdateAllGameObjectsEventType");

	/// <summary>
	/// An event that informs the receiver to do a logic update, without actually coupling to the receiver
	/// </summary>
	class UpdateAllGameObjectsEvent final : public Event
	{
	public:
		/// <summary>
		/// Create a Logic Update Event
		/// </summary>
		UpdateAllGameObjectsEvent();

		/// <summary>
		/// All events should have a string version
		/// </summary>
		/// <returns></returns>
		std::string ToString() override;
	};
}

#endif