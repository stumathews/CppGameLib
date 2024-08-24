#pragma once
#include "JsonEventSerializationManager.h"
#include "events/IEventSerializationManager.h"

namespace gamelib
{
	class XmlEventSerializationManager : public IEventSerializationManager
	{
	public:
		 std::shared_ptr<PlayerMovedEvent> DeserializePlayerMovedEvent(std::string serializedMessage) override;
		 std::string SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target) override;
		 std::string CreateRequestPlayerDetailsMessage() override;
		 std::string CreatePongMessage() override;
		 static std::string CreateDummyXml();
		 std::string CreatePingMessage() override;
		 std::string SerializeControllerMoveEvent(std::shared_ptr<ControllerMoveEvent> object, std::string target) override;
		 std::string CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target) override;
		 std::string SerializeStartNetworkLevelEvent(std::shared_ptr<StartNetworkLevelEvent> evt, std::string target) override;
		 std::shared_ptr<StartNetworkLevelEvent> DeserializeStartNetworkLevel(std::string serializedMessage) override;
		 std::string CreateRequestPlayerDetailsMessageResponse(const std::string& target) override;

	 private:
			JsonEventSerializationManager json;
	};
}

