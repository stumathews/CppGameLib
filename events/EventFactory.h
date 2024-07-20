#pragma once

#include <events/PlayerMovedEvent.h>
#include <events/IEventSerializationManager.h>
#include <memory>
#include <string>
#include <events/NetworkTrafficReceivedEvent.h>
#include <events/SceneChangedEvent.h>
#include <net/NetworkPlayer.h>

#include "IEventSubscriber.h"
#include "ReliableUdpPacketReceivedEvent.h"
#include "UpdateAllGameObjectsEvent.h"
#include "UpdateProcessesEvent.h"
#include "net/Message.h"

namespace gamelib
{
	class SubscriberHandledEvent;
	class NetworkPlayerJoinedEvent;
	class AddGameObjectToCurrentSceneEvent;
	class GameObject;

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
		[[nodiscard]] static std::shared_ptr<AddGameObjectToCurrentSceneEvent> CreateAddToSceneEvent(const std::shared_ptr<GameObject>& obj);
		[[nodiscard]] std::shared_ptr<SceneChangedEvent> CreateSceneChangedEventEvent(int newLevel) const;
		[[nodiscard]] std::shared_ptr<Event> CreateGenericEvent(const EventId& id, const std::string& origin) const;
	};
}

