#include "SerializationManager.h"
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

using namespace json11;

namespace gamelib
{
	SerializationManager::SerializationManager(const gamelib::Encoding desiredEncoding = Encoding::Json)
	{
		Encoding = desiredEncoding;
		Initialize();
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

		if(evt->Id.PrimaryId == PlayerMovedEventTypeEventId.PrimaryId) { return CreatePlayerMovedEventMessage(evt, target);}
		if(evt->Id.PrimaryId == ControllerMoveEventId.PrimaryId) { return CreateControllerMoveEventMessage(evt, target);}
		if(evt->Id.PrimaryId == StartNetworkLevelEventId.PrimaryId) { return CreateStartNetworkLevelMessage(evt, target);}
		return CreateUnknownEventMessage(evt, target);
		
	}

	std::shared_ptr<Event> SerializationManager::Deserialize(const MessageHeader& messageHeader, const std::string&
	                                                         serializedMessage) const
	{
		std::shared_ptr<Event> event = nullptr;

		// TODO: Update the list of messages (serialised events) that we can be consumed by the network clients, i.e deserialized
		
		if(messageHeader.MessageType == StartNetworkLevelEventId.Name)
		{
			return To<Event>(eventSerialization->DeserializeStartNetworkLevel(serializedMessage));
		}

		if(messageHeader.MessageType == PlayerMovedEventTypeEventId.Name)
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

	std::string SerializationManager::CreateRequestPlayerDetailsMessageResponse(std::string target)
	{
		// Send our Nick to the server

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
		// Change the serialization format here
		switch(Encoding)
		{
		case Encoding::Json: 			
			eventSerialization = std::make_shared<JsonEventSerializationManager>();
			break;
		case Encoding::Xml:
			eventSerialization = std::make_shared<XmlEventSerializationManager>();
			break;
		case Encoding::BitPacked:
			eventSerialization = std::make_shared<BitPackedEventSerializationManager>();
			break;
		case Encoding::None: break;
		default:
			
			throw std::exception(std::string("Unknown serialization format").c_str());
		}
		return true;
	}

}