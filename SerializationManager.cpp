#include "SerializationManager.h"
#include <string>
#include <json/json11.h>
#include <net/MessageHeader.h>
#include <events/Event.h>
#include <json/JsonEventSerializationManager.h>
#include <events/ControllerMoveEvent.h>
#include <events/PlayerMovedEvent.h>
#include <events/StartNetworkLevelEvent.h>

using namespace json11;

namespace gamelib
{
	SerializationManager* SerializationManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new SerializationManager();
			instance->Initialize();
		}
		return instance;
	}

	SerializationManager::SerializationManager()
	{
	}
	
	SerializationManager* SerializationManager::instance = nullptr;

	
	SerializationManager::~SerializationManager()
	{		
		instance = nullptr;
	}

	MessageHeader SerializationManager::GetMessageHeader(const std::string& serializedMessage) const
	{
		std::string error;
		const auto parsedJson = Json::parse(serializedMessage, error);

		MessageHeader header;
		header.MessageType = parsedJson["messageType"].string_value();
		header.MessageTarget = parsedJson["nickname"].string_value();

		return header;
	}

	std::string SerializationManager::Serialize(const std::shared_ptr<Event>& evt, const std::string& target) const
	{
		// TODO: Update the list of events that can be sent over the network

		switch(evt->Type)  // NOLINT(clang-diagnostic-switch-enum)
		{
			case EventType::PlayerMovedEventType:
				return CreatePlayerMovedEventMessage(evt, target);
			case EventType::ControllerMoveEvent:
				return CreateControllerMoveEventMessage(evt, target);
			case EventType::StartNetworkLevel:
				return CreateStartNetworkLevelMessage(evt, target);
			default:
				return CreateUnknownEventMessage(evt, target);
		}
	}

	std::shared_ptr<Event> SerializationManager::Deserialize(const MessageHeader& messageHeader, const std::string&
	                                                         serializedMessage) const
	{
		std::shared_ptr<Event> event = nullptr;

		// TODO: Update the list of messages (serialised events) that we can be consumed by the network clients, i.e deserialized

		// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
		// ReSharper disable once CppIncompleteSwitchStatement
		switch(FromString(messageHeader.MessageType))  // NOLINT(clang-diagnostic-switch)
		{
		case EventType::PlayerMovedEventType:
			return std::dynamic_pointer_cast<Event>(eventSerialization->DeserializePlayerMovedEvent(serializedMessage));
		case EventType::StartNetworkLevel:
			return std::dynamic_pointer_cast<Event>(eventSerialization->DeserializeStartNetworkLevel(serializedMessage));
		}
		
		return event;
	}

	std::string SerializationManager::CreatePlayerMovedEventMessage(const std::shared_ptr<Event>& event, const std::string
	                                                                & target) const
	{
		const auto playerMovedEvent = std::dynamic_pointer_cast<PlayerMovedEvent>(event);
		return eventSerialization->SerializePlayerMovedEvent(playerMovedEvent, target);
	}

	std::string SerializationManager::CreateControllerMoveEventMessage(const std::shared_ptr<Event>& evt, const std::string
	                                                                   & target) const
	{
		const auto controllerMoveEvent = std::dynamic_pointer_cast<ControllerMoveEvent>(evt);
		return eventSerialization->SerializeControllerMoveEvent(controllerMoveEvent, target);
	}

	std::string SerializationManager::CreateStartNetworkLevelMessage(const std::shared_ptr<Event>& evt, const std::string
	                                                                 & target) const
	{
		const auto startNetworkLevelEvent = std::dynamic_pointer_cast<StartNetworkLevelEvent>(evt);
		return eventSerialization->SerializeStartNetworkLevelEvent(startNetworkLevelEvent, target);
	}
		
	std::string SerializationManager::CreateUnknownEventMessage(const std::shared_ptr<Event>& evt, const std::string&
	                                                            target) const
	{
		return eventSerialization->CreateUnknownEventMessage(evt, target);
	}

	std::string SerializationManager::CreateRequestPlayerDetailsMessage() const
	{
		return eventSerialization->CreateRequestPlayerDetailsMessage();		
	}

	std::string SerializationManager::CreateRequestPlayerDetailsMessageResponse(std::string target)
	{
		// Send our Nick to the server

		const Json payload = Json::object
		{
			{ "messageType", "requestPlayerDetails" },
			{ "nickname", target}
		};			


		return payload.dump();
	}

	std::string SerializationManager::CreatePongMessage() const
	{
		return eventSerialization->CreatePongMessage();
	}

	std::string SerializationManager::CreatePingMessage() const
	{
		return eventSerialization->CreatePingMessage();
	}

	std::string SerializationManager::UpdateTarget(const std::string& target, const std::string& serializedMessage) const
	{
		std::string error;
		const auto parsedJson = Json::parse(serializedMessage, error);
		Json::object json_obj = parsedJson.object_items();
		json_obj["nickname"] = target;
		const Json another_json = json_obj;
		return another_json.dump();
	}

	bool SerializationManager::Initialize()
	{		
		// Change the serializtion format here
		eventSerialization = std::make_shared<JsonEventSerializationManager>();
		return true;
	}

}