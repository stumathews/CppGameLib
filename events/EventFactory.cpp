#include "EventFactory.h"
#include <json/JsonEventSerializationManager.h>
#include <memory>
#include <events/StartNetworkLevelEvent.h>
#include <net/NetworkPlayer.h>
#include <events/NetworkPlayerJoinedEvent.h>
#include <events/IEventSubscriber.h>
#include <events/AddGameObjectToCurrentSceneEvent.h>

#include "UpdateAllGameObjectsEvent.h"
#include "UpdateProcessesEvent.h"

namespace gamelib
{
	EventFactory* EventFactory::Get()
	{
		if (instance == nullptr)
		{
			instance = new EventFactory();
		}
		return instance;
	}

	EventFactory::EventFactory()
	{
		this->eventSerializationManager = std::make_shared<JsonEventSerializationManager>();
	}

	EventFactory* EventFactory::instance = nullptr;

	
	EventFactory::~EventFactory()
	{		
		instance = nullptr;
	}

	std::shared_ptr<Event> EventFactory::CreateGenericEvent(const EventId& id, const std::string& origin = "") const
	{
		auto event = std::make_shared<Event>(id);
		event->Origin = origin;
		return event;
	}
	

	std::shared_ptr<PlayerMovedEvent> EventFactory::CreatePlayerMovedEvent(const Direction direction, const std::string
	                                                                       & target) const
	{
		return std::make_shared<PlayerMovedEvent>(direction);
	}

	std::shared_ptr<PlayerMovedEvent> EventFactory::CreatePlayerMovedEvent(const std::string& serializedMessage) const
	{
		return eventSerializationManager->DeserializePlayerMovedEvent(serializedMessage);
	}

	std::shared_ptr<NetworkTrafficReceivedEvent> EventFactory::CreateNetworkTrafficReceivedEvent(const std::string&
		message, const std::string& identifier, const int bytesReceived, const std::string& origin) const
	{
		auto event = std::make_shared<NetworkTrafficReceivedEvent>();
		event->Message = message;
		event->Identifier = identifier;
		event->BytesReceived = bytesReceived;
		event->Origin = origin; // Origin of the event is the where the event was emitted from
		return event;
	}

	std::shared_ptr<SceneChangedEvent> EventFactory::CreateLevelEvent(const int level) const
	{
		return std::make_shared<SceneChangedEvent>(level);
	}

	std::shared_ptr<UpdateAllGameObjectsEvent> EventFactory::CreateUpdateAllGameObjectsEvent() const
	{
		return std::make_shared<UpdateAllGameObjectsEvent>();
	}

	std::shared_ptr<UpdateProcessesEvent> EventFactory::CreateUpdateProcessesEvent() const
	{
		return std::make_shared<UpdateProcessesEvent>();
	}

	std::shared_ptr<StartNetworkLevelEvent> EventFactory::CreateStartNetworkLevelEvent(const int level) const
	{
		return std::make_shared<StartNetworkLevelEvent>(level);
	}

	std::shared_ptr<Event> EventFactory::CreateNetworkPlayerJoinedEvent(const NetworkPlayer& player) const
	{
		return std::make_shared<NetworkPlayerJoinedEvent>(player);
	}

	std::shared_ptr<ControllerMoveEvent> EventFactory::CreateControllerMoveEvent(Direction direction,
		ControllerMoveEvent::KeyState keyState) const
	{
		return std::make_shared<ControllerMoveEvent>(direction, keyState);
	}

	std::shared_ptr<AddGameObjectToCurrentSceneEvent> EventFactory::CreateAddToSceneEvent(const std::shared_ptr<GameObject> & obj)
	{
		return std::make_shared<AddGameObjectToCurrentSceneEvent>(obj);
	}

	std::shared_ptr<SceneChangedEvent> EventFactory::CreateSceneChangedEventEvent(const int newLevel) const
	{
		return std::make_shared<SceneChangedEvent>(newLevel);
	}
}
