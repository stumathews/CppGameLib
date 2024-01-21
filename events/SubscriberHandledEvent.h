#pragma once
#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	
	const static EventId EventHandledEventId(SubscriberEventHandled, "EventHandled");
	

	class SubscriberHandledEvent final : public Event
	{		
	public:		
		explicit SubscriberHandledEvent(IEventSubscriber* subscriber, const std::shared_ptr<Event> event, const unsigned long deltaMs)  // NOLINT(performance-unnecessary-value-param)
			: Event(EventHandledEventId), Subscriber(subscriber), Evt(event), DeltaMs(deltaMs)
		{
		}

		std::string ToString() override { return "SubscriberHandledEvent"; }
		IEventSubscriber* Subscriber;
		const std::shared_ptr<Event> Evt;
		unsigned long DeltaMs;
	};
}
