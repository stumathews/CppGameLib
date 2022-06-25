#pragma once
#include <events\IEventSerializationManager.h>
#include <json\json11.h>

namespace gamelib
{
	class JsonEventSerializationManager :  public IEventSerializationManager
	{
		// Inherited via IEventSerializationManager
		virtual std::shared_ptr<PlayerMovedEvent> DeserializePlayerMovedEvent(std::string serializedMessage) override;
		virtual std::string SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object, std::string target) override;

		// Inherited via IEventSerializationManager
		virtual std::string CreateRequestPlayerDetailsMessage() override;
		virtual std::string CreatePongMessage() override;
	};
}

