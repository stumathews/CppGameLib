#include "JsonEventSerializationManager.h"
#include <events/PlayerMovedEvent.h>
#include <events/ControllerMoveEvent.h>
#include <events/StartNetworkLevelEvent.h>
#include "events/Events.h"
#include "events/NetworkTrafficReceivedEvent.h"

using namespace json11;
namespace gamelib
{
	std::string JsonEventSerializationManager::SerializePlayerMovedEvent(const std::shared_ptr<PlayerMovedEvent> object, std::string target)
	{
		const Json payload = Json::object
		{
			{ "messageType", object->Id.Name },
			{ "direction", ToString(object->Direction) },
			{ "nickname", target }
		};

		return payload.dump();
	}

    std::shared_ptr<PlayerMovedEvent> JsonEventSerializationManager::DeserializePlayerMovedEvent(const std::string serializedMessage)
    {
        std::string error;
        const auto payload = Json::parse(serializedMessage.c_str(), error);
        const auto& direction = payload["direction"].string_value();
        return std::make_shared<PlayerMovedEvent>(FromDirectionString(direction));
    }	

	std::shared_ptr<StartNetworkLevelEvent> JsonEventSerializationManager::DeserializeStartNetworkLevel(const std::string serializedMessage)
	{
		std::string error;
		const auto payload = Json::parse(serializedMessage.c_str(), error);
        const auto& level = payload["level"].int_value();
        return std::make_shared<StartNetworkLevelEvent>(level);
	
	}

	std::string JsonEventSerializationManager::SerializeControllerMoveEvent(const std::shared_ptr<ControllerMoveEvent> object, std::string target)
    {
	    const Json payload = Json::object
		{
			{ "messageType", object->Id.Name },
			{ "direction", ToString(object->Direction) },
			{ "nickname", target }
		};

		return payload.dump();        
    }

    	

    std::string JsonEventSerializationManager::CreateRequestPlayerDetailsMessage()
    {
        // Request player details
        const Json sendPayload = Json::object 
		{
			{ "messageType", "requestPlayerDetails" }
		};

		return sendPayload.dump();
    }

	std::string JsonEventSerializationManager::SerializeStartNetworkLevelEvent(const std::shared_ptr<StartNetworkLevelEvent> evt, std::string target)
	{
		const Json payload = Json::object
		{
			{ "messageType", evt->Id.Name },
			{ "level", evt->Level },
			{ "nickname", target }
		};

		return payload.dump();
	}

	std::string JsonEventSerializationManager::CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target) 
	{
		const Json payload = Json::object
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

		const Json sendPayload = Json::object
		{
			{ "messageType", "pong" },
			{ "isHappy", true },
			{ "eventType", NetworkTrafficReceivedEventId.Id },
			{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
			{ "ages", Json::array{ 1, 2, 3 } },
			{ "fish", Json::object{ { "yo", "sushi" } } }
		};

		return sendPayload.dump();
    }

	std::string JsonEventSerializationManager::CreatePingMessage()
	{
		const Json payload = Json::object{
			{ "messageType", "ping" },
			{ "isHappy", false },
			{ "eventType", NetworkTrafficReceivedEventId.Id },
			{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
			{ "ages", Json::array{ 1, 2, 3 } },
			{ "fish", Json::object{ { "yo", "sushi" } } }
		};

		return payload.dump();
	}
	
}