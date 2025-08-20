#pragma once

#include <memory>
#include <string>
#include <cppgamenetlib/NetworkPlayer.h>


#include "cppgamenetlib/Message.h"


#include "ControllerMoveEvent.h"
#include "character/Direction.h"
#include "cppgamenetlib/Rtt.h"


namespace gamenetlib
{
	class StartNetworkLevelEvent;
	class ReliableUdpPacketRttCalculatedEvent;
	class ReliableUdpAckPacketEvent;
	class ReliableUdpPacketLossDetectedEvent;
	class ReliableUdpCheckSumFailedEvent;
	class ReliableUdpPacketReceivedEvent;
	class NetworkTrafficReceivedEvent;
	class IEventSerializationManager;
}

namespace gamelib
{
	class SubscriberHandledEvent;
	class AddGameObjectToCurrentSceneEvent;
	class GameObject;
	class StartNetworkLevelEvent;
	class PlayerMovedEvent;
	class UpdateAllGameObjectsEvent;
	class UpdateProcessesEvent;
	class IEventSubscriber;
	class SceneChangedEvent;

	class EventFactory
	{
	protected:
			static EventFactory* instance;
	private:
			std::shared_ptr<gamenetlib::IEventSerializationManager> eventSerializationManager;
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
		[[nodiscard]] std::shared_ptr<gamenetlib::NetworkTrafficReceivedEvent> CreateNetworkTrafficReceivedEvent(const std::string& message, const std::string& identifier, const int bytesReceived, const std::string& origin) const;
		[[nodiscard]] std::shared_ptr<SceneChangedEvent> CreateLevelEvent(int level) const;
		[[nodiscard]] std::shared_ptr<UpdateAllGameObjectsEvent> CreateUpdateAllGameObjectsEvent() const;
		[[nodiscard]] std::shared_ptr<UpdateProcessesEvent> CreateUpdateProcessesEvent() const;
		[[nodiscard]] std::shared_ptr<gamenetlib::StartNetworkLevelEvent> CreateStartNetworkLevelEvent(int level) const;
		[[nodiscard]] std::shared_ptr<Event> CreateNetworkPlayerJoinedEvent(const gamenetlib::NetworkPlayer& player) const;
		[[nodiscard]] std::shared_ptr<ControllerMoveEvent> CreateControllerMoveEvent(Direction direction, ControllerMoveEvent::KeyState keyState) const;
		[[nodiscard]] std::shared_ptr<Event> CreateSubscriberHandledEvent(IEventSubscriber* value, const std::shared_ptr<Event>& event, unsigned long deltaMs) const;
		[[nodiscard]] std::shared_ptr<gamenetlib::ReliableUdpPacketReceivedEvent> CreateReliableUdpPacketReceived(std::shared_ptr<gamenetlib::Message> message) const;
		[[nodiscard]] std::shared_ptr<gamenetlib::ReliableUdpCheckSumFailedEvent> CreateReliableUdpCheckSumFailedEvent(std::shared_ptr<gamenetlib::Message> failedMessage) const;
		[[nodiscard]] std::shared_ptr<gamenetlib::ReliableUdpPacketLossDetectedEvent> CreateReliableUdpPacketLossDetectedEvent(const std::shared_ptr<gamenetlib::Message>& messageBundle) const;
		[[nodiscard]] std::shared_ptr<gamenetlib::ReliableUdpAckPacketEvent> CreateReliableUdpAckPacketEvent(const std::shared_ptr<gamenetlib::Message>& message, bool isSent) const;
		[[nodiscard]]std::shared_ptr<gamenetlib::ReliableUdpPacketRttCalculatedEvent> CreateReliableUdpPacketRttCalculatedEvent(const std::shared_ptr<gamenetlib::Message>& message, gamenetlib::Rtt rtt) const;
		[[nodiscard]] static std::shared_ptr<AddGameObjectToCurrentSceneEvent> CreateAddToSceneEvent(const std::shared_ptr<GameObject>& obj);
		[[nodiscard]] std::shared_ptr<SceneChangedEvent> CreateSceneChangedEventEvent(int newLevel) const;
		[[nodiscard]] std::shared_ptr<Event> CreateGenericEvent(const EventId& id, const std::string& origin) const;
	};
}

