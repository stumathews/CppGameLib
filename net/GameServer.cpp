#include "GameServer.h"
#include <events/EventManager.h>
#include <net/PeerInfo.h>
#include <net/Networking.h>
#include <json/json11.h>
#include <events/EventFactory.h>
#include <SerializationManager.h>
#include <net/IGameServerConnection.h>
#include <net/TcpGameServerConnection.h>
#include <net/UdpGameServerConnection.h>

#include "events/StartNetworkLevelEvent.h"
#include "util/SettingsManager.h"

using namespace json11;


namespace gamelib
{
	GameServer::GameServer(const std::string& address, const std::string& port)
	{
		this->address = address;
		this->port = port;			
		this->isTcp = true;
		this->gameServerConnection = nullptr;
	}

	void GameServer::Initialize()
	{
		Logger::Get()->LogThis("Game Server Starting...");

		serializationManager = SerializationManager::Get();
		_eventManager = EventManager::Get();
		networking = Networking::Get();
		_eventFactory = EventFactory::Get();
		isTcp = SettingsManager::Get()->GetBool("networking", "isTcp");

		Logger::Get()->LogThis(this->isTcp ? "Using TCP" : "Using UDP");

		// Create a new server socket connection, either TCP or UDP depending on game configuration
		gameServerConnection = this->isTcp ? std::dynamic_pointer_cast<IGameServerConnection>(
				                       std::make_shared<TcpGameServerConnection>(address, port))
										   : std::dynamic_pointer_cast<IGameServerConnection>(
											   std::make_shared<UdpGameServerConnection>(address, port));
		
		gameServerConnection->Initialize();
		gameServerConnection->Create();

		// Get our nickname
		nickname  = SettingsManager::Get()->GetString("networking", "nickname");

		// We're interested in some of the our own game's events
		_eventManager->SubscribeToEvent(PlayerMovedEventTypeEventId, this);
		_eventManager->SubscribeToEvent(ControllerMoveEventId, this);
		_eventManager->SubscribeToEvent(StartNetworkLevelEventId, this);
	}
	
	void GameServer::Listen() const
	{		
		gameServerConnection->Listen();
	}
		
	void GameServer::CheckForPlayerTraffic() const
	{
		gameServerConnection->CheckForPlayerTraffic();		
	}

	std::vector<std::shared_ptr<Event>> GameServer::HandleEvent(const std::shared_ptr<Event> evt, unsigned long deltaMs)
	{
		gameServerConnection->SendEventToAllPlayers(serializationManager->Serialize(evt, nickname)); 

		return {};
	}

	std::string GameServer::GetSubscriberName()
	{
		return "Game Server";
	}
		
	GameServer::~GameServer() = default;
}
