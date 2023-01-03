#include "EventFactory.h"
#include <json/JsonEventSerializationManager.h>
#include <memory>
#include <events/StartNetworkLevelEvent.h>
#include <net/NetworkPlayer.h>
#include <events/NetworkPlayerJoinedEvent.h>
#include <events/IEventSubscriber.h>
#include <events/AddGameObjectToCurrentSceneEvent.h>

namespace gamelib
{
	EventFactory* EventFactory::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new EventFactory();
			Initialize();
		}
		return Instance;
	}

	EventFactory::EventFactory()
	{
		this->eventSerializationManager = std::make_shared<JsonEventSerializationManager>();
	}

	bool EventFactory::Initialize()
	{		
		return false;
	}

	EventFactory* EventFactory::Instance = nullptr;

	
	EventFactory::~EventFactory()
	{		
		Instance = nullptr;
	}

	std::shared_ptr<Event> EventFactory::CreateGenericEvent(EventType type) const
	{
		return std::make_shared<Event>(type);
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
		message, const std::string& identifier, const int bytesReceived) const
	{
		auto event = std::make_shared<NetworkTrafficReceivedEvent>(
			EventType::NetworkTrafficReceived);
		event->Message = message;
		event->Identifier = identifier;
		event->BytesReceived = bytesReceived;
		return event;
	}
	std::shared_ptr<SceneChangedEvent> EventFactory::CreateLevelEvent(const int level) const
	{
		return std::make_shared<SceneChangedEvent>(level);
	}

	std::shared_ptr<StartNetworkLevelEvent> EventFactory::CreateStartNetworkLevelEvent(const int level) const
	{
		return std::make_shared<StartNetworkLevelEvent>(level);
	}

	std::shared_ptr<Event> EventFactory::CreateNetworkPlayerJoinedEvent(const NetworkPlayer& player) const
	{
		return std::make_shared<NetworkPlayerJoinedEvent>(player);
	}

	std::shared_ptr<AddGameObjectToCurrentSceneEvent> EventFactory::CreateAddToSceneEvent(const std::shared_ptr<GameObject>
		& obj) const
	{
		return std::make_shared<AddGameObjectToCurrentSceneEvent>(obj);
	}
}
