#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Event.h"

namespace gamelib
{
	/// <summary>
	/// Objects that can subscribe and raise events but implement this interface
	/// </summary>
	class IEventSubscriber
	{
		public:

		/// <summary>
		/// All subscribers can recieve events
		/// </summary>
		/// <param name="evt">Incoming event to handle</param>
		/// <returns>List of generated events while handling current event</returns>
		virtual std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt, unsigned long deltaMs = 0) = 0;

		/// <summary>
		/// Each subscriber must have a name
		/// </summary>
		/// <returns></returns>
		virtual std::string GetSubscriberName() = 0;

		/// <summary>
		/// Each subscriber must have an ID
		/// </summary>
		/// <returns></returns>
		virtual int GetSubscriberId() = 0;
	};
}

