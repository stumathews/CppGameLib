#pragma once
#include <string>
#include <memory>

namespace gamelib
{
	class PlayerMovedEvent;
	class ControllerMoveEvent;
	class Event;
	class StartNetworkLevelEvent;
	class IEventSerializationManager	 
	{
	public:
		virtual ~IEventSerializationManager() = default;
		virtual std::shared_ptr<PlayerMovedEvent> DeserializePlayerMovedEvent(std::string serializedMessage) = 0;
		virtual std::string SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target) = 0;
		virtual std::string CreateRequestPlayerDetailsMessage() = 0;
		virtual std::string CreatePongMessage() = 0;
		virtual std::string CreatePingMessage() = 0;
		virtual std::string SerializeControllerMoveEvent(std::shared_ptr<ControllerMoveEvent> object, std::string target) = 0;
		virtual std::string CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target) = 0;
		virtual std::string SerializeStartNetworkLevelEvent(std::shared_ptr<StartNetworkLevelEvent> evt, std::string target) = 0;
		virtual std::shared_ptr<StartNetworkLevelEvent> DeserializeStartNetworkLevel(std::string serializedMessage) = 0;
		virtual std::string CreateRequestPlayerDetailsMessageResponse(const std::string& target) = 0;
	};
}

