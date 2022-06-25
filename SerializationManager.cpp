#include "SerializationManager.h"
#include <string>
#include <json/json11.h>
#include <net/MessageHeader.h>
#include <events/Event.h>
#include <json/JsonEventSerializationManager.h>
#include <events/ControllerMoveEvent.h>
#include <events/PlayerMovedEvent.h>

using namespace json11;

namespace gamelib
{
	SerializationManager* SerializationManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new SerializationManager();
			Instance->Initialize();
		}
		return Instance;
	}

	SerializationManager::SerializationManager()
	{
	}
	
	SerializationManager* SerializationManager::Instance = nullptr;

	
	SerializationManager::~SerializationManager()
	{		
		Instance = nullptr;
	}

	MessageHeader SerializationManager::GetMessageHeader(std::string serializedMessage)
	{
		std::string error;
		auto parsedJson = Json::parse(serializedMessage, error);

		MessageHeader header;
		header.MessageType = parsedJson["messageType"].string_value();
		header.MessageTarget = parsedJson["nickname"].string_value();

		return header;
	}

	std::shared_ptr<Event> SerializationManager::Deserialize(MessageHeader messageHeader, std::string serializedMessage)
	{
		std::shared_ptr<Event> event = nullptr;

		switch(FromString(messageHeader.MessageType))
		{
		case EventType::PlayerMovedEventType:
			return std::dynamic_pointer_cast<Event>(eventSerialization->DeserializePlayerMovedEvent(serializedMessage));
			break;
		}
		
		return event;
	}

	std::string SerializationManager::CreatePlayerMovedEventMessage(std::shared_ptr<Event> evt, std::string target)
	{
		auto playerMovedEvent = std::dynamic_pointer_cast<PlayerMovedEvent>(evt);
		return eventSerialization->SerializePlayerMovedEvent(playerMovedEvent, target);
	}

	std::string SerializationManager::CreateControllerMoveEventMessage(std::shared_ptr<Event> evt, std::string target)
	{
		auto controllerMoveEvent = std::dynamic_pointer_cast<ControllerMoveEvent>(evt);
		return eventSerialization->SerializeControllerMoveEvent(controllerMoveEvent, target);
	}
	
	std::string SerializationManager::Serialize(std::shared_ptr<Event> evt, std::string target)
	{
		switch(evt->type)
		{
			case gamelib::EventType::PlayerMovedEventType:
				return CreatePlayerMovedEventMessage(evt, target);
			break;
			case gamelib::EventType::ControllerMoveEvent:
				return CreateControllerMoveEventMessage(evt, target);
				break;
			default:
				return CreateUnknownEventMessage(evt, target);
		}

		return std::string();
		
	}



	std::string SerializationManager::CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target)
	{
		return eventSerialization->CreateUnknownEventMessage(evt, target);
	}

	std::string SerializationManager::CreateRequestPlayerDetailsMessage()
	{
		return eventSerialization->CreateRequestPlayerDetailsMessage();		
	}

	std::string SerializationManager::CreateRequestPlayerDetailsMessageResponse(std::string target)
	{
		// Send our Nick to the server

		Json payload = Json::object
		{
			{ "messageType", "requestPlayerDetails" },
			{ "nickname", target}
		};			


		return payload.dump();
	}

	std::string SerializationManager::CreatePongMessage()
	{
		return eventSerialization->CreatePongMessage();
	}

	std::string SerializationManager::CreatePingMessage()
	{
		return eventSerialization->CreatePingMessage();
	}

	bool SerializationManager::Initialize()
	{		
		// Change the serializtion format here
		eventSerialization = std::shared_ptr<JsonEventSerializationManager>(new JsonEventSerializationManager());
		return true;
	}

}