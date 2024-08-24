#pragma once
#include <memory>
#include <string>
#include <net/MessageHeader.h>
#include <events/Event.h>
#include <events/IEventSerializationManager.h>

#include "net/BitPacker.h"

namespace gamelib
{

	enum class Encoding
	{
		None,
		Json,
		Xml,
		BitPacked
	};

	class SerializationManager
	{
	
		std::shared_ptr<IEventSerializationManager> eventSerialization;
				[[nodiscard]] std::string CreatePlayerMovedEventMessage(const std::shared_ptr<Event>& event, const std::string& target) const;
				[[nodiscard]] std::string CreateControllerMoveEventMessage(const std::shared_ptr<Event>& evt, const std::string& target) const;
				[[nodiscard]] std::string CreateStartNetworkLevelMessage(const std::shared_ptr<Event>& evt, const std::string& target) const;
		
				bool Initialize();

	public:
			SerializationManager (Encoding desiredEncoding);
				[[nodiscard]] MessageHeader GetMessageHeader(const std::string& serializedMessage) const;

				[[nodiscard]] std::shared_ptr<Event> Deserialize(const MessageHeader& messageHeader, const std::string& serializedMessage) const;
				[[nodiscard]] std::string Serialize(const std::shared_ptr<Event>& evt, const std::string& target) const;

				[[nodiscard]] std::string CreateUnknownEventMessage(const std::shared_ptr<Event>& evt, const std::string& target) const;
				[[nodiscard]] std::string CreateRequestPlayerDetailsMessage() const;
				std::string CreateRequestPlayerDetailsMessageResponse(std::string target);
				[[nodiscard]] std::string CreatePongMessage() const;

				[[nodiscard]] std::string CreatePingMessage() const;

				[[nodiscard]] std::string UpdateTarget(const std::string& target, const std::string& serialisedMessage) const;


			// Cannot copy an SerializationManager
			SerializationManager(SerializationManager const&) = delete;

			// Cannot assign to an SerializationManager
			void operator=(SerializationManager const&) = delete;

			// Underlying serialization method
			inline static Encoding Encoding = Encoding::None;
	
	};
}

