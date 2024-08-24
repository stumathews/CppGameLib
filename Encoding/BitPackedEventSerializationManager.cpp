#include "BitPackedEventSerializationManager.h"

#include "events/NetworkTrafficReceivedEvent.h"
#include "net/Fish.h"
#include "net/PingMessage.h"

namespace gamelib
{

	std::shared_ptr<PlayerMovedEvent> BitPackedEventSerializationManager::DeserializePlayerMovedEvent(
		std::string serializedMessage)
	{
		return json.DeserializePlayerMovedEvent(serializedMessage);
	}

	std::string BitPackedEventSerializationManager::SerializePlayerMovedEvent(std::shared_ptr<PlayerMovedEvent> object,
		std::string target)
	{
		return json.SerializePlayerMovedEvent(object, target);
	}

	std::string BitPackedEventSerializationManager::CreateRequestPlayerDetailsMessage()
	{
		return json.CreateRequestPlayerDetailsMessage();
	}

	std::string BitPackedEventSerializationManager::CreatePongMessage()
	{
		return json.CreatePingMessage();
	}

	std::string BitPackedEventSerializationManager::CreatePingMessage()
	{
		
		BitPacker bitPacker(networkBuffer, NetworkBufferSize); // 32-bit packer
		
		PingMessage pingMessage;
		pingMessage.Type = "ping";
		pingMessage.IsHappy = true;
		pingMessage.EventType = NetworkTrafficReceivedEventId.PrimaryId;
		pingMessage.Names = { "Stuart", "Jenny", "bruce" };
		pingMessage.Ages = {1, 2,3 };
		pingMessage.Fish = Fish("Neemo", "Mathews");

		pingMessage.Write(bitPacker);

		return std::string(bitPacker.ToBytes().data());		
	}

	std::string BitPackedEventSerializationManager::SerializeControllerMoveEvent(
		std::shared_ptr<ControllerMoveEvent> object, std::string target)
	{
		return json.SerializeControllerMoveEvent(object, target);
	}

	std::string BitPackedEventSerializationManager::CreateUnknownEventMessage(std::shared_ptr<Event> evt,
		std::string target)
	{
		return json.CreateUnknownEventMessage(evt, target);
	}

	std::string BitPackedEventSerializationManager::SerializeStartNetworkLevelEvent(
		std::shared_ptr<StartNetworkLevelEvent> evt, std::string target)
	{
		return json.SerializeStartNetworkLevelEvent(evt, target);
	}

	std::shared_ptr<StartNetworkLevelEvent> BitPackedEventSerializationManager::DeserializeStartNetworkLevel(
		std::string serializedMessage)
	{
		return json.DeserializeStartNetworkLevel(serializedMessage);
	}

	std::string BitPackedEventSerializationManager::CreateRequestPlayerDetailsMessageResponse(const std::string& target)
	{
		return json.CreateRequestPlayerDetailsMessageResponse(target);
	}
}
