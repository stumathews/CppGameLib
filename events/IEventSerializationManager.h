#pragma once
#include <string>
#include <memory>
namespace gamelib
{
	class PlayerMovedEvent;
	class ControllerMoveEvent;
	class Event;
	class IEventSerializationManager
	{
	public:
		virtual std::shared_ptr<PlayerMovedEvent> DeserializePlayerMovedEvent(std::string serializedMessage) = 0;
		virtual std::string SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target) = 0;
		virtual std::string CreateRequestPlayerDetailsMessage() = 0;
		virtual std::string CreatePongMessage() = 0;
		virtual std::string CreatePingMessage() = 0;
		virtual std::string SerializeControllerMoveEvent(std::shared_ptr<ControllerMoveEvent> object, std::string target) = 0;
		virtual std::string CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target) = 0;
	};
}

