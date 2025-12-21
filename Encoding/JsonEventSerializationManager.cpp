#include "JsonEventSerializationManager.h"
#include <events/PlayerMovedEvent.h>
#include <events/ControllerMoveEvent.h>
#include <events/StartNetworkLevelEvent.h>
#include "character/DirectionUtils.h"
#include "events/NetworkTrafficReceivedEvent.h"
#include "net/PingMessage.h"
#include "net/PongMessage.h"

using namespace json11;

namespace gamelib
{
	std::string JsonEventSerializationManager::SerializePlayerMovedEvent(const std::shared_ptr<PlayerMovedEvent> object, std::string target)
	{
		const Json payload = Json::object
		{
			{ "messageType", object->Id.Name },
			{ "direction", DirectionUtils::ToString(object->direction) },
			{ "nickname", target }
		};

		return payload.dump();
	}

    std::shared_ptr<PlayerMovedEvent> JsonEventSerializationManager::DeserializePlayerMovedEvent(const std::string serializedMessage)
    {
        std::string error;
        const auto payload = Json::parse(serializedMessage.c_str(), error);
        const auto& direction = payload["direction"].string_value();

        return std::make_shared<PlayerMovedEvent>(DirectionUtils::FromDirectionString(direction));
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
			{ "direction", DirectionUtils::ToString(object->direction) },
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

	std::string JsonEventSerializationManager::CreateRequestPlayerDetailsMessageResponse(const std::string& target)
	{
		const Json payload = Json::object
		{
			{ "messageType", "requestPlayerDetails" },
			{ "nickname", target}
		};			


		return payload.dump();
	}

	std::string JsonEventSerializationManager::CreatePongMessage()
    {
		PongMessage pongMessage;
			pongMessage.Type = "pong";
			pongMessage.isHappy = true;
			pongMessage.eventType = NetworkTrafficReceivedEventId.PrimaryId;
			pongMessage.names = { "Stuart", "Jenny", "bruce" };
			pongMessage.ages = {1, 2,3 };
			pongMessage.fish = Fish("Neemo", "Mathews");


		const Json sendPayload = Json::object
		{
			{ "messageType", pongMessage.Type },
			{ "isHappy", pongMessage.isHappy },
			{ "eventType", pongMessage.eventType },
			{ "names",  pongMessage.names },
			{ "ages",  pongMessage.ages  },
			{ "fish", Json::object{ { pongMessage.fish.Name, pongMessage.fish.Surname } } }
		};

		return sendPayload.dump();
    }

	std::string JsonEventSerializationManager::CreatePingMessage()
	{
		PingMessage pingMessage;
			pingMessage.Type = "ping";
			pingMessage.IsHappy = true;
			pingMessage.EventType = NetworkTrafficReceivedEventId.PrimaryId;
			pingMessage.Names = { "Stuart", "Jenny", "bruce" };
			pingMessage.Ages = {1, 2,3 };
			pingMessage.TheFish = Fish("Neemo", "Mathews");


		const Json sendPayload = Json::object
		{
			{ "messageType", pingMessage.Type },
			{ "isHappy", pingMessage.IsHappy },
			{ "eventType", pingMessage.EventType },
			{ "names", pingMessage.Names },
			{ "ages",  pingMessage.Ages },
			{ "fish", Json::object{ { pingMessage.TheFish.Name, pingMessage.TheFish.Surname } } }
		};
		return sendPayload.dump();
	}
	
}