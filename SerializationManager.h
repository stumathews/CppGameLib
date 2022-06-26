#pragma once
#include <memory>
#include <string>
#include <net/MessageHeader.h>
#include <events/Event.h>
#include <events/IEventSerializationManager.h>

namespace gamelib
{
	class SerializationManager
	{
		protected:
				static SerializationManager* Instance;
	private:
		std::shared_ptr<IEventSerializationManager> eventSerialization;
		std::string CreatePlayerMovedEventMessage(std::shared_ptr<Event> event, std::string target);
		std::string CreateControllerMoveEventMessage(std::shared_ptr<Event> evt, std::string target);
		public:
			static SerializationManager* Get();
			SerializationManager();
			~SerializationManager();
			MessageHeader GetMessageHeader(std::string serializedMessage);

			std::shared_ptr<Event> Deserialize(MessageHeader messageHeader, std::string serializedMessage);		
			std::string Serialize(std::shared_ptr<Event> evt, std::string target);

			std::string CreateUnknownEventMessage(std::shared_ptr<Event> evt, std::string target);
			std::string CreateRequestPlayerDetailsMessage();
			std::string CreateRequestPlayerDetailsMessageResponse(std::string target);
			std::string CreatePongMessage();

			std::string CreatePingMessage();

			std::string UpdateTarget(std::string target, std::string serialisedMessage);

			bool Initialize();

			// Cannot copy an SerializationManager
			SerializationManager(SerializationManager const&) = delete;

			// Cannot assign to an SerializationManager
			void operator=(SerializationManager const&) = delete;
	
	};
}

