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
		public:
			static SerializationManager* Get();
			SerializationManager();
			~SerializationManager();
			MessageHeader GetMessageHeader(std::string serializedMessage);

			std::shared_ptr<Event> DeserializeToEvent(MessageHeader messageHeader, std::string serializedMessage);

			std::string CreatePlayerMovedEventMessage(std::shared_ptr<Event> event, std::string target);

			std::string CreateEventMessage(std::shared_ptr<Event> evt, std::string target);

			std::string CreateRequestPlayerDetailsMessage();
			std::string CreateRequestPlayerDetailsMessageResponse(std::string target);
			std::string CreatePongMessage();

			bool Initialize();

			// Cannot copy an SerializationManager
			SerializationManager(SerializationManager const&) = delete;

			// Cannot assign to an SerializationManager
			void operator=(SerializationManager const&) = delete;
	
	};
}

