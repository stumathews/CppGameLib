#pragma once
#include <events\IEventSerializationManager.h>
#include <json\json11.h>

namespace gamelib
{
	class JsonEventSerializationManager :  public IEventSerializationManager
	{
		virtual std::shared_ptr<PlayerMovedEvent> DeserializePlayerMovedEvent(std::string serializedMessage) override;
		virtual std::shared_ptr<StartNetworkLevelEvent> DeserializeStartNetworkLevel(std::string serializedMessage) override;
		virtual std::string SerializeControllerMoveEvent(std::shared_ptr<ControllerMoveEvent> object, std::string target) override;
		virtual std::string SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target) override;
		virtual std::string CreateRequestPlayerDetailsMessage() override;
		virtual std::string SerializeStartNetworkLevelEvent(std::shared_ptr<StartNetworkLevelEvent> evt, std::string target) override;
		virtual std::string CreatePongMessage() override;
		virtual std::string CreatePingMessage() override;
		virtual std::string CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target) override;
	};
}

