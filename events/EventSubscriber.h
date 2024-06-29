#pragma once
#ifndef EVENTSUBSCRIBER_H
#define EVENTSUBSCRIBER_H

#include <list>

#include "IEventSubscriber.h"
namespace gamelib
{
	class EventSubscriber : virtual public IEventSubscriber
	{
	public:
									
		void RaiseEvent(const std::shared_ptr<Event>& theEvent);
		int GetSubscriberId() override;		
		void SubscribeToEvent(const EventId& eventId);
		void UnsubscribeSubscribeToEvent(const EventId& eventId);
		bool SubscribesTo(const EventId& eventId);

		std::list<EventId> EventSubscriptions;
	};
}

#endif
