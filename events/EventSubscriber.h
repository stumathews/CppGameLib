#pragma once
#include <list>

#include "IEventSubscriber.h"
namespace gamelib
{
	class EventSubscriber : public IEventSubscriber
	{
	public:
									
		void RaiseEvent(const std::shared_ptr<Event>& theEvent);
		int GetSubscriberId() override;		
		void SubscribeToEvent(const EventId& eventId);
		void UnsubscribeSubscribeToEvent(const EventId& eventId);
		std::list<EventId> EventSubscriptions;
		bool SubscribesTo(const EventId& eventId);
	};
}

