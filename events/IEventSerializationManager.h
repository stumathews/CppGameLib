#pragma once
#include <events/PlayerMovedEvent.h>
#include <string>
#include <memory>
namespace gamelib
{
	class IEventSerializationManager
	{
	public:
		//virtual std::string Serialize
		virtual std::shared_ptr<PlayerMovedEvent> DeserializePlayerMovedEvent(std::string serializedMessage) = 0;
		virtual std::string SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target) = 0;
		virtual std::string CreateRequestPlayerDetailsMessage() = 0;
		virtual std::string CreatePongMessage() = 0;
	};
}

