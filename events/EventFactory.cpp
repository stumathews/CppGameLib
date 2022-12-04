#include "EventFactory.h"
#include <json/JsonEventSerializationManager.h>
#include <memory>
#include <exceptions/EngineException.h>
#include <events/StartNetworkLevelEvent.h>
#include <net/NetworkPlayer.h>
#include <events/NetworkPlayerJoinedEvent.h>
#include <events/IEventSubscriber.h>
#include <events\AddGameObjectToCurrentSceneEvent.h>

namespace gamelib
{
	EventFactory* EventFactory::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new EventFactory();
			Instance->Initialize();
		}
		return Instance;
	}

	EventFactory::EventFactory()
	{
		this->eventSerializationManager = std::shared_ptr<JsonEventSerializationManager>(new JsonEventSerializationManager());
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

	std::shared_ptr<Event> EventFactory::CreateGenericEvent(EventType type)
	{
		return std::make_shared<gamelib::Event>(type);
	}

	std::shared_ptr<PlayerMovedEvent> EventFactory::CreatePlayerMovedEvent(gamelib::Direction direction, std::string target)
	{
		return std::shared_ptr<PlayerMovedEvent>(new PlayerMovedEvent(direction));
	}

	std::shared_ptr<PlayerMovedEvent> EventFactory::CreatePlayerMovedEvent(std::string serializedMessage)
	{
		return eventSerializationManager->DeserializePlayerMovedEvent(serializedMessage);
	}

	
	std::shared_ptr<NetworkTrafficRecievedEvent> EventFactory::CreateNetworkTrafficReceivedEvent(std::string message, std::string identifier, int bytesReceived)
	{
		auto event = std::shared_ptr<gamelib::NetworkTrafficRecievedEvent>(new NetworkTrafficRecievedEvent(gamelib::EventType::NetworkTrafficReceived));
		event->Message = message;
		event->Identifier = identifier;
		event->bytesReceived = bytesReceived;
		return event;
	}
	std::shared_ptr<SceneChangedEvent> EventFactory::CreateLevelEvent(int level)
	{
		return std::shared_ptr<SceneChangedEvent>(new SceneChangedEvent(level));
	}

	std::shared_ptr<StartNetworkLevelEvent> EventFactory::CreateStartNetworkLevelEvent(int level)
	{
		return std::shared_ptr<StartNetworkLevelEvent>(new StartNetworkLevelEvent(level));
	}

	std::shared_ptr<NetworkPlayerJoinedEvent> EventFactory::CreateNetworkPlayerJoinedEvent(NetworkPlayer player)
	{
		return std::shared_ptr<NetworkPlayerJoinedEvent>(new NetworkPlayerJoinedEvent(player));
	}

	std::shared_ptr<AddGameObjectToCurrentSceneEvent> EventFactory::CreateAddToSceneEvent(std::shared_ptr<GameObject> obj)
	{
		return std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(obj));
	}
}
