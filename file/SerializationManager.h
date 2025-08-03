#pragma once
#include <memory>
#include <string>
#include <net/MessageHeader.h>
#include "net/BitPacker.h"

namespace gamelib
{
	class Event;
	class IEventSerializationManager;

	enum class Encoding : uint8_t
	{
		none,
		json,
		xml,
		bit_packed
	};

	class SerializationManager
	{
	
		std::shared_ptr<IEventSerializationManager> eventSerialization;
		[[nodiscard]] std::string CreatePlayerMovedEventMessage(const std::shared_ptr<Event>& event, const std::string& target) const;
		[[nodiscard]] std::string CreateControllerMoveEventMessage(const std::shared_ptr<Event>& evt, const std::string& target) const;
		[[nodiscard]] std::string CreateStartNetworkLevelMessage(const std::shared_ptr<Event>& evt, const std::string& target) const;

		void Initialize();

	public:
		explicit SerializationManager (Encoding desiredEncoding);

		~SerializationManager() = default;

		[[nodiscard]] static MessageHeader GetMessageHeader(const std::string& serializedMessage);
		[[nodiscard]] std::shared_ptr<Event> Deserialize(const MessageHeader& messageHeader, const std::string& serializedMessage) const;
		[[nodiscard]] std::string SerializeEvent(const std::shared_ptr<Event>& evt, const std::string& target) const;
		[[nodiscard]] std::string CreateUnknownEventMessage(const std::shared_ptr<Event>& evt, const std::string& target) const;
		[[nodiscard]] std::string CreateRequestPlayerDetailsMessage() const;
		[[nodiscard]] std::string CreateRequestPlayerDetailsMessageResponse(const std::string& target) const;
		[[nodiscard]] std::string CreatePongMessage() const;
		[[nodiscard]] std::string CreatePingMessage() const;
		[[nodiscard]] static std::string UpdateTarget(const std::string& target, const std::string& serialisedMessage);

		// Cannot copy/move an SerializationManager
		SerializationManager(SerializationManager const&) = delete;
		SerializationManager(SerializationManager&&) = delete;

		// Cannot assign to an SerializationManager
		void operator=(SerializationManager const&) = delete;

		// Cannot do move assignment
		SerializationManager& operator=(SerializationManager&& other) = delete;

		// Underlying serialization method
		inline static auto Encoding = Encoding::none;	
	};
}

