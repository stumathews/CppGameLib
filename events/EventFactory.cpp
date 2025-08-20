#include "EventFactory.h"
#include <memory>
#include <cppgamenetlib/StartNetworkLevelEvent.h>
#include <cppgamenetlib/NetworkPlayer.h>
#include <cppgamenetlib/NetworkPlayerJoinedEvent.h>
#include <events/IEventSubscriber.h>
#include <events/AddGameObjectToCurrentSceneEvent.h>
#include "UpdateAllGameObjectsEvent.h"
#include "UpdateProcessesEvent.h"
#include <events/SubscriberHandledEvent.h>
#include <cppgamenetlib/ReliableUdpPacketReceivedEvent.h>
#include <cppgamenetlib/JsonEventSerializationManager.h>
#include <events/PlayerMovedEvent.h>
#include <cppgamenetlib/NetworkTrafficReceivedEvent.h>
#include <cppgamenetlib/ReliableUdpAckPacketEvent.h>
#include <cppgamenetlib/ReliableUdpPacketRttCalculatedEvent.h>
#include "IEventSubscriber.h"
#include <cppgamenetlib/ReliableUdpCheckSumFailedEvent.h>
#include <cppgamenetlib/ReliableUdpPacketReceivedEvent.h>
#include <cppgamenetlib\ReliableUdpPacketLossDetectedEvent.h>
#include "UpdateAllGameObjectsEvent.h"
#include "UpdateProcessesEvent.h"
#include <events/SceneChangedEvent.h>
#include <cppgamenetlib\StartNetworkLevelEvent.h>

namespace gamelib
{
	EventFactory* EventFactory::Get()
	{
		if (instance == nullptr)
		{
			instance = new EventFactory();
		}
		return instance;
	}

	EventFactory::EventFactory()
	{
		this->eventSerializationManager = std::make_shared<gamenetlib::JsonEventSerializationManager>();
	}

	EventFactory* EventFactory::instance = nullptr;

	
	EventFactory::~EventFactory()
	{		
		instance = nullptr;
	}

	std::shared_ptr<Event> EventFactory::CreateGenericEvent(const EventId& id, const std::string& origin = "") const
	{
		auto event = std::make_shared<Event>(id);
		event->Origin = origin;
		return event;
	}
	

	std::shared_ptr<PlayerMovedEvent> EventFactory::CreatePlayerMovedEvent(const Direction direction, const std::string
	                                                                       & target) const
	{
		return std::make_shared<PlayerMovedEvent>(direction);
	}

	std::shared_ptr<PlayerMovedEvent> EventFactory::CreatePlayerMovedEvent(const std::string& serializedMessage) const
	{
		return eventSerializationManager->DeserializePlayerMovedEvent(serializedMessage);
	}

	std::shared_ptr<gamenetlib::NetworkTrafficReceivedEvent> EventFactory::CreateNetworkTrafficReceivedEvent(const std::string&
	                                                                                                         message, const std::string& identifier, const int bytesReceived, const std::string& origin) const
	{
		auto event = std::make_shared<gamenetlib::NetworkTrafficReceivedEvent>();
		event->Message = message;
		event->Identifier = identifier;
		event->BytesReceived = bytesReceived;
		event->Origin = origin; // Origin of the event is the where the event was emitted from
		return event;
	}

	std::shared_ptr<SceneChangedEvent> EventFactory::CreateLevelEvent(const int level) const
	{
		return std::make_shared<SceneChangedEvent>(level);
	}

	std::shared_ptr<UpdateAllGameObjectsEvent> EventFactory::CreateUpdateAllGameObjectsEvent() const
	{
		return std::make_shared<UpdateAllGameObjectsEvent>();
	}

	std::shared_ptr<UpdateProcessesEvent> EventFactory::CreateUpdateProcessesEvent() const
	{
		return std::make_shared<UpdateProcessesEvent>();
	}

	std::shared_ptr<gamenetlib::StartNetworkLevelEvent> EventFactory::CreateStartNetworkLevelEvent(const int level) const
	{
		return std::make_shared<gamenetlib::StartNetworkLevelEvent>(level);
	}

	std::shared_ptr<Event> EventFactory::CreateNetworkPlayerJoinedEvent(const gamenetlib::NetworkPlayer& player) const
	{
		return std::make_shared<gamenetlib::NetworkPlayerJoinedEvent>(player);
	}

	std::shared_ptr<ControllerMoveEvent> EventFactory::CreateControllerMoveEvent(Direction direction,
		ControllerMoveEvent::KeyState keyState) const
	{
		return std::make_shared<ControllerMoveEvent>(direction, keyState);
	}

	std::shared_ptr<Event> EventFactory::CreateSubscriberHandledEvent(IEventSubscriber* value,
	                                                                  const std::shared_ptr<Event>& event,
	                                                                  unsigned long deltaMs) const
	{
		return std::make_shared<SubscriberHandledEvent>(value, event, deltaMs);
	}

	std::shared_ptr<gamenetlib::ReliableUdpPacketReceivedEvent> EventFactory::CreateReliableUdpPacketReceived(std::shared_ptr<gamenetlib::Message> message) const
	{
		return std::make_shared<gamenetlib::ReliableUdpPacketReceivedEvent>(message);
	}

	std::shared_ptr<gamenetlib::ReliableUdpCheckSumFailedEvent> EventFactory::CreateReliableUdpCheckSumFailedEvent(std::shared_ptr<gamenetlib::Message> failedMessage) const
	{
		return std::make_shared<gamenetlib::ReliableUdpCheckSumFailedEvent>(failedMessage);
	}

	std::shared_ptr<gamenetlib::ReliableUdpPacketLossDetectedEvent> EventFactory::CreateReliableUdpPacketLossDetectedEvent(const std::shared_ptr<gamenetlib::Message>& messageBundle) const
	{
		return std::make_shared<gamenetlib::ReliableUdpPacketLossDetectedEvent>(messageBundle);
	}

	std::shared_ptr<gamenetlib::ReliableUdpAckPacketEvent> EventFactory::CreateReliableUdpAckPacketEvent(
		const std::shared_ptr<gamenetlib::Message>& message, bool isSent) const
	{
		return std::make_shared<gamenetlib::ReliableUdpAckPacketEvent>(message, isSent);
	}

	std::shared_ptr<gamenetlib::ReliableUdpPacketRttCalculatedEvent> EventFactory::CreateReliableUdpPacketRttCalculatedEvent(
		const std::shared_ptr<gamenetlib::Message>& message, gamenetlib::Rtt rtt) const
	{
		return std::make_shared<gamenetlib::ReliableUdpPacketRttCalculatedEvent>(message, rtt);
	}

	std::shared_ptr<AddGameObjectToCurrentSceneEvent> EventFactory::CreateAddToSceneEvent(const std::shared_ptr<GameObject> & obj)
	{
		return std::make_shared<AddGameObjectToCurrentSceneEvent>(obj);
	}

	std::shared_ptr<SceneChangedEvent> EventFactory::CreateSceneChangedEventEvent(const int newLevel) const
	{
		return std::make_shared<SceneChangedEvent>(newLevel);
	}
}
