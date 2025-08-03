#pragma once

#include <memory>
#include <string>
#include <net/NetworkPlayer.h>


#include "net/Message.h"


#include "ControllerMoveEvent.h"
#include "character/Direction.h"
#include "net/Rtt.h"


namespace gamelib
{
	class ReliableUdpPacketRttCalculatedEvent;
	class ReliableUdpAckPacketEvent;
	class ReliableUdpPacketLossDetectedEvent;
	class ReliableUdpCheckSumFailedEvent;
	class ReliableUdpPacketReceivedEvent;
	class SubscriberHandledEvent;
	class NetworkPlayerJoinedEvent;
	class AddGameObjectToCurrentSceneEvent;
	class GameObject;
	class IEventSerializationManager;
	class StartNetworkLevelEvent;
	class PlayerMovedEvent;
	class NetworkTrafficReceivedEvent;
	class UpdateAllGameObjectsEvent;
	class UpdateProcessesEvent;
	class IEventSubscriber;
	class SceneChangedEvent;

	class EventFactory
	{
	protected:
			static EventFactory* instance;
	private:
			std::shared_ptr<IEventSerializationManager> eventSerializationManager;
	public:
		static EventFactory* Get();
		EventFactory();

		// Cannot copy an EventFactory
		EventFactory(EventFactory const&) = delete;
	
		~EventFactory();
		
		// Cannot assign to an EventFactory
		void operator=(EventFactory const&) = delete;

		[[nodiscard]] std::shared_ptr<PlayerMovedEvent> CreatePlayerMovedEvent(Direction direction, const std::string& target = "") const;
		[[nodiscard]] std::shared_ptr<PlayerMovedEvent> CreatePlayerMovedEvent(const std::string& serializedMessage) const;
		[[nodiscard]] std::shared_ptr<NetworkTrafficReceivedEvent> CreateNetworkTrafficReceivedEvent(const std::string& message, const std::string& identifier, const int bytesReceived, const std::string& origin) const;
		[[nodiscard]] std::shared_ptr<SceneChangedEvent> CreateLevelEvent(int level) const;
		[[nodiscard]] std::shared_ptr<UpdateAllGameObjectsEvent> CreateUpdateAllGameObjectsEvent() const;
		[[nodiscard]] std::shared_ptr<UpdateProcessesEvent> CreateUpdateProcessesEvent() const;
		[[nodiscard]] std::shared_ptr<StartNetworkLevelEvent> CreateStartNetworkLevelEvent(int level) const;
		[[nodiscard]] std::shared_ptr<Event> CreateNetworkPlayerJoinedEvent(const NetworkPlayer& player) const;
		[[nodiscard]] std::shared_ptr<ControllerMoveEvent> CreateControllerMoveEvent(Direction direction, ControllerMoveEvent::KeyState keyState) const;
		[[nodiscard]] std::shared_ptr<Event> CreateSubscriberHandledEvent(IEventSubscriber* value, const std::shared_ptr<Event>& event, unsigned long deltaMs) const;
		[[nodiscard]] std::shared_ptr<ReliableUdpPacketReceivedEvent> CreateReliableUdpPacketReceived(std::shared_ptr<Message> message) const;
		[[nodiscard]] std::shared_ptr<ReliableUdpCheckSumFailedEvent> CreateReliableUdpCheckSumFailedEvent(std::shared_ptr<Message> failedMessage) const;
		[[nodiscard]] std::shared_ptr<ReliableUdpPacketLossDetectedEvent> CreateReliableUdpPacketLossDetectedEvent(const std::shared_ptr<Message>& messageBundle) const;
		[[nodiscard]] std::shared_ptr<ReliableUdpAckPacketEvent> CreateReliableUdpAckPacketEvent(const std::shared_ptr<Message>& message, bool isSent) const;
		[[nodiscard]]std::shared_ptr<ReliableUdpPacketRttCalculatedEvent> CreateReliableUdpPacketRttCalculatedEvent(const std::shared_ptr<Message>& message, Rtt rtt) const;
		[[nodiscard]] static std::shared_ptr<AddGameObjectToCurrentSceneEvent> CreateAddToSceneEvent(const std::shared_ptr<GameObject>& obj);
		[[nodiscard]] std::shared_ptr<SceneChangedEvent> CreateSceneChangedEventEvent(int newLevel) const;
		[[nodiscard]] std::shared_ptr<Event> CreateGenericEvent(const EventId& id, const std::string& origin) const;
	};
}

