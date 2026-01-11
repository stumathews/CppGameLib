#include "SerializationManager.h"
#include <cassert>
#include <string>
#include <json/json11.h>
#include <net/MessageHeader.h>
#include <events/Event.h>
#include <events/ControllerMoveEvent.h>
#include <events/PlayerMovedEvent.h>
#include <events/StartNetworkLevelEvent.h>
#include "Encoding/BitPackedEventSerializationManager.h"
#include "Encoding/JsonEventSerializationManager.h"
#include "Encoding/XmlEventSerializationManager.h"
#include "net/Message.h"
#include "utils/Utils.h"
#include <exceptions/EngineException.h>

using namespace json11;

namespace gamelib
{
	SerializationManager::SerializationManager(Encoding desiredEncoding)
	{
		TheEncoding = desiredEncoding;
		Initialize();
	}

	void SerializationManager::Initialize()
	{
		assert(TheEncoding == Encoding::json || TheEncoding == Encoding::xml || TheEncoding == Encoding::bit_packed || TheEncoding == Encoding::none);

		switch (TheEncoding)
		{
			case Encoding::json: 
				eventSerialization = std::make_shared<JsonEventSerializationManager>();
				break;
			case Encoding::xml:
				eventSerialization = std::make_shared<XmlEventSerializationManager>();
				break;
			case Encoding::bit_packed:
				eventSerialization = std::make_shared<BitPackedEventSerializationManager>();
				break;
			case Encoding::none:
				THROW(99,"Unknown serialization format","SerializationManager");
		}
	}

	MessageHeader SerializationManager::GetMessageHeader(const std::string& serializedMessage)
	{
		std::string jsonParseError;
		const auto jsonObject = Json::parse(serializedMessage, jsonParseError);

		if (!jsonParseError.empty())
		{
			throw std::runtime_error("Failed to parse JSON: " + jsonParseError);
		}

		MessageHeader header
		{
			.TheMessageType = jsonObject["messageType"].string_value(),
			.MessageTarget = jsonObject["nickname"].string_value(),
		};

		return header;
	}

	std::string SerializationManager::SerializeEvent(const std::shared_ptr<Event>& evt, const std::string& target) const
	{
		// TODO: Update the list of events that can be sent over the network

		if(evt->Id.PrimaryId == PlayerMovedEventTypeEventId.PrimaryId) { return CreatePlayerMovedEventMessage(evt, target);}
		if(evt->Id.PrimaryId == ControllerMoveEventId.PrimaryId) { return CreateControllerMoveEventMessage(evt, target);}
		if(evt->Id.PrimaryId == StartNetworkLevelEventId.PrimaryId) { return CreateStartNetworkLevelMessage(evt, target);}

		return CreateUnknownEventMessage(evt, target);
		
	}

	std::shared_ptr<Event> SerializationManager::Deserialize(const MessageHeader& messageHeader, const std::string&
	                                                         serializedMessage) const
	{
		std::shared_ptr<Event> event = nullptr;

		// TODO: Update the list of messages (serialised events) that we can be consumed by the network clients, i.e., deserialized
		
		if(messageHeader.TheMessageType == StartNetworkLevelEventId.Name)
		{
			return To<Event>(eventSerialization->DeserializeStartNetworkLevel(serializedMessage));
		}

		if(messageHeader.TheMessageType == PlayerMovedEventTypeEventId.Name)
		{
			return To<Event>(eventSerialization->DeserializePlayerMovedEvent(serializedMessage));
		}
		
		return event;
	}

	std::string SerializationManager::CreatePlayerMovedEventMessage(const std::shared_ptr<Event>& event, const std::string
	                                                                & target) const
	{
		const auto playerMovedEvent = To<PlayerMovedEvent>(event);
		return eventSerialization->SerializePlayerMovedEvent(playerMovedEvent, target);
	}

	std::string SerializationManager::CreateControllerMoveEventMessage(const std::shared_ptr<Event>& evt, const std::string
	                                                                   & target) const
	{
		const auto controllerMoveEvent = To<ControllerMoveEvent>(evt);
		return eventSerialization->SerializeControllerMoveEvent(controllerMoveEvent, target);
	}

	std::string SerializationManager::CreateStartNetworkLevelMessage(const std::shared_ptr<Event>& evt, const std::string
	                                                                 & target) const
	{
		const auto startNetworkLevelEvent = To<StartNetworkLevelEvent>(evt);
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

	std::string SerializationManager::CreateRequestPlayerDetailsMessageResponse(const std::string& target) const
	{
		return eventSerialization->CreateRequestPlayerDetailsMessageResponse(target);		
	}

	std::string SerializationManager::CreatePongMessage() const
	{
		return eventSerialization->CreatePongMessage();
	}

	std::string SerializationManager::CreatePingMessage() const
	{
		return eventSerialization->CreatePingMessage();
	}

	std::string SerializationManager::UpdateTarget(const std::string& target, const std::string& serialisedMessage)
	{
		std::string jsonParseError;
		const auto jsonObject = Json::parse(serialisedMessage, jsonParseError);

		if (!jsonParseError.empty())
		{
			throw std::runtime_error("Failed to parse JSON: " + jsonParseError);
		}

		Json::object jsonObj = jsonObject.object_items();
		jsonObj["nickname"] = target;
		const Json anotherJson = jsonObj;
		return anotherJson.dump();
	}
}
