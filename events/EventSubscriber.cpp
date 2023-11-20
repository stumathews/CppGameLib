#include "EventSubscriber.h"
#include <common/Common.h>
#include "EventManager.h"

int gamelib::EventSubscriber::GetSubscriberId()
{
	return get_random_int(0, INT_MAX);
}

void gamelib::EventSubscriber::RaiseEvent(const std::shared_ptr<Event>& theEvent)
{
	EventManager::Get()->RaiseEvent(theEvent, this);
}

void gamelib::EventSubscriber::SubscribeToEvent(const EventId& eventId)
{
	EventManager::Get()->SubscribeToEvent(eventId, this);
	EventSubscriptions.push_back(eventId);
}

void gamelib::EventSubscriber::UnsubscribeSubscribeToEvent(const EventId& eventId)
{		
	EventManager::Get()->Unsubscribe(eventId.PrimaryId);
	EventSubscriptions.remove(eventId);
}

bool gamelib::EventSubscriber::SubscribesTo(const EventId& eventId)
{
	return std::find(begin(EventSubscriptions), end(EventSubscriptions), eventId) != end(EventSubscriptions);
}
