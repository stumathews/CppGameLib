#pragma once
#include <events/IEventSerializationManager.h>
#include <json/json11.h>

namespace gamelib
{
	class PlayerMovedEvent;
	class ControllerMoveEvent;
	class Event;
	class StartNetworkLevelEvent;
	class IEventSerializationManager;

	class JsonEventSerializationManager final :  public IEventSerializationManager
	{
	public:
		std::shared_ptr<PlayerMovedEvent> DeserializePlayerMovedEvent(std::string serializedMessage) override;
		std::shared_ptr<StartNetworkLevelEvent> DeserializeStartNetworkLevel(std::string serializedMessage) override;
		std::string SerializeControllerMoveEvent(std::shared_ptr<ControllerMoveEvent> object, std::string target) override;
		std::string SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target) override;
		std::string CreateRequestPlayerDetailsMessage() override;
		std::string SerializeStartNetworkLevelEvent(std::shared_ptr<StartNetworkLevelEvent> evt, std::string target) override;
		std::string CreatePongMessage() override;
		std::string CreatePingMessage() override;
		std::string CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target) override;
		std::string CreateRequestPlayerDetailsMessageResponse(const std::string& target) override;
	};
}

