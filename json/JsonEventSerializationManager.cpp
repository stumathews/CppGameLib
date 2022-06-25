#include "JsonEventSerializationManager.h"
#include <events/PlayerMovedEvent.h>
#include <events/ControllerMoveEvent.h>

using namespace json11;
namespace gamelib
{
    std::shared_ptr<PlayerMovedEvent> gamelib::JsonEventSerializationManager::DeserializePlayerMovedEvent(std::string serializedMessage)
    {
        std::string error;
		auto payload = Json::parse(serializedMessage.c_str(), error);
        const auto& direction = payload["direction"].string_value();
        return std::shared_ptr<PlayerMovedEvent>(new PlayerMovedEvent(gamelib::FromDirectionString(direction)));
    }	

	std::string gamelib::JsonEventSerializationManager::SerializeControllerMoveEvent(std::shared_ptr<ControllerMoveEvent> object, std::string target)
    {
        Json payload = Json::object
		{
			{ "messageType", ToString(object->type) },
			{ "direction", ToString(object->direction) },
			{ "nickname", target }
		};

		return payload.dump();        
    }

    std::string gamelib::JsonEventSerializationManager::SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target)
    {
        Json payload = Json::object
		{
			{ "messageType", ToString(object->type) },
			{ "direction", ToString(object->direction) },
			{ "nickname", target }
		};

		return payload.dump();
    }	

    std::string JsonEventSerializationManager::CreateRequestPlayerDetailsMessage()
    {
        // Request player details
		Json sendPayload = Json::object 
		{
			{ "messageType", "requestPlayerDetails" }
		};

		return sendPayload.dump();
    }

	std::string JsonEventSerializationManager::CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target) 
	{
		 Json payload = Json::object
		{
			{ "messageType", "unknown" },
			{ "nickname", target }
		};

		return payload.dump();
	}

    std::string JsonEventSerializationManager::CreatePongMessage()
    {
		/*

		Json payload = Json::object
		{
		{ "messageType", "ping" },
		{ "isHappy", false },
		{ "eventType", (int)gamelib::EventType::NetworkTrafficReceived },
		{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
		{ "ages", Json::array{ 1, 2, 3 } },
		{ "fish", Json::object{ { "yo", "sushi" } } }
		};

		*/

		Json sendPayload = Json::object
		{
			{ "messageType", "pong" },
			{ "isHappy", true },
			{ "eventType", (int)gamelib::EventType::NetworkTrafficReceived },
			{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
			{ "ages", Json::array{ 1, 2, 3 } },
			{ "fish", Json::object{ { "yo", "sushi" } } }
		};

		return sendPayload.dump();
    }

	std::string JsonEventSerializationManager::CreatePingMessage()
	{
		Json payload = Json::object{
			{ "messageType", "ping" },
			{ "isHappy", false },
			{ "eventType", (int)gamelib::EventType::NetworkTrafficReceived },
			{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
			{ "ages", Json::array{ 1, 2, 3 } },
			{ "fish", Json::object{ { "yo", "sushi" } } }
		};

		return payload.dump();
	}
	
}