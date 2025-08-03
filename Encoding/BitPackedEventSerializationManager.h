#ifndef BITPACKED_EVENT_SERIALIZATION_MANAGER_H
#define BITPACKED_EVENT_SERIALIZATION_MANAGER_H
#pragma once
#include "JsonEventSerializationManager.h"
#include "events/IEventSerializationManager.h"

namespace gamelib
{
	class StartNetworkLevelEvent;
	class Event;
	class ControllerMoveEvent;
	class PlayerMovedEvent;

	class BitPackedEventSerializationManager : public IEventSerializationManager
	{
	public:
		 std::shared_ptr<PlayerMovedEvent> DeserializePlayerMovedEvent(std::string serializedMessage) override;
		 std::string SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target) override;
		 std::string CreateRequestPlayerDetailsMessage() override;
		 std::string CreatePongMessage() override;
		 std::string CreatePingMessage() override;
		 std::string SerializeControllerMoveEvent(std::shared_ptr<ControllerMoveEvent> object, std::string target) override;
		 std::string CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target) override;
		 std::string SerializeStartNetworkLevelEvent(std::shared_ptr<StartNetworkLevelEvent> evt, std::string target) override;
		 std::shared_ptr<StartNetworkLevelEvent> DeserializeStartNetworkLevel(std::string serializedMessage) override;
		 std::string CreateRequestPlayerDetailsMessageResponse(const std::string& target) override;

	 private:
		JsonEventSerializationManager json;
		constexpr static int NetworkBufferSize = 128;
		uint32_t networkBuffer[NetworkBufferSize] = {0}; // 512 byte network buffer
	};
}

#endif

