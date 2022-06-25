#include "JsonEventSerializationManager.h"

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

    std::string gamelib::JsonEventSerializationManager::SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target)
    {
        Json payload = Json::object
		{
			{ "messageType", ToString(object->type) },
			{ "direction", ToString(object->direction) },
			{ "nickname", target }
		};

		auto json_str = payload.dump();
        return json_str;
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

    std::string JsonEventSerializationManager::CreatePongMessage()
    {
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
	
}