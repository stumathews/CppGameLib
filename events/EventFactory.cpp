#include "EventFactory.h"
#include <json/JsonEventSerializationManager.h>
#include <memory>
#include <exceptions/EngineException.h>

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
		auto event = std::shared_ptr<gamelib::NetworkTrafficRecievedEvent>(new NetworkTrafficRecievedEvent(gamelib::EventType::NetworkTrafficReceived, 0));
		event->Message = message;
		event->Identifier = identifier;
		event->bytesReceived = bytesReceived;
		return event;
	}
}
