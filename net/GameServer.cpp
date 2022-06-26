#include "GameServer.h"
#include <events/EventManager.h>
#include <net/PeerInfo.h>
#include <events/NetworkTrafficRecievedEvent.h>
#include <net/Networking.h>
#include <json/json11.h>
#include <events/PlayerMovedEvent.h>
#include <events/EventFactory.h>
#include <SerializationManager.h>
#include <net/MessageHeader.h>
#include <net/IGameServerConnection.h>
#include <net/TcpGameServerConnection.h>
#include <net/UdpGameServerConnection.h>

using namespace json11;


namespace gamelib
{
	GameServer::GameServer(std::string address, std::string port)
	{
		this->address = address;
		this->port = port;			
		this->isTcp = true;
		this->gameServerConnection = nullptr;
	}

	void GameServer::Initialize()
	{
		Logger::Get()->LogThis("Game Server Starting.");

		this->serializationManager = SerializationManager::Get();
		this->eventManager = EventManager::Get();
		this->networking = Networking::Get();
		this->eventFactory = EventFactory::Get();

		this->isTcp = SettingsManager::Get()->GetBool("networking", "isTcp");

		// Create a new server socket connection
		gameServerConnection = this->isTcp ? std::dynamic_pointer_cast<IGameServerConnection>(std::shared_ptr<TcpGameServerConnection>(new TcpGameServerConnection(address, port)))
										   : std::dynamic_pointer_cast<IGameServerConnection>(std::shared_ptr<UdpGameServerConnection>(new UdpGameServerConnection(address, port)));
		
		gameServerConnection->Initialize();

		// Read client nickname
		this->nickname  = SettingsManager::Get()->GetString("networking", "nickname");

		// Send our events off on the network
		eventManager->SubscribeToEvent(EventType::PlayerMovedEventType, this);
		eventManager->SubscribeToEvent(EventType::ControllerMoveEvent, this);
		eventManager->SubscribeToEvent(EventType::Fire, this);
		
	}
	
	void GameServer::Listen()
	{		
		gameServerConnection->Listen();
	}
		
	void GameServer::CheckForPlayerTraffic()
	{
		gameServerConnection->CheckForPlayerTraffic();		
	}

	std::vector<std::shared_ptr<Event>> GameServer::HandleEvent(std::shared_ptr<Event> evt)
	{				
		gameServerConnection->SendEventToAllPlayers(serializationManager->Serialize(evt, nickname)); 

		return std::vector<std::shared_ptr<Event>>();
	}

	std::string GameServer::GetSubscriberName()
	{
		return "Game Server";
	}
		
	GameServer::~GameServer()
	{
		
	}
}
