#include "GameServer.h"
#include <events/EventManager.h>
#include <net/Networking.h>
#include <json/json11.h>
#include <events/EventFactory.h>
#include <file/SerializationManager.h>
#include <net/IGameServerConnection.h>
#include <net/TcpGameServerConnection.h>
#include <net/UdpGameServerConnection.h>
#include "events/StartNetworkLevelEvent.h"
#include "utils/Utils.h"

using namespace json11;


namespace gamelib
{
	GameServer::GameServer(const std::string& address, const std::string& port,
	                       const std::shared_ptr<IGameServerConnection> gameServerConnection,
	                       const std::string& nickName, const gamelib::Encoding wireFormat)
	{		
		this->gameServerConnection = gameServerConnection;
		this->nickname = nickName;
		this->Address = address;
		this->Port = port;
		this->Encoding = wireFormat;
	}

	void GameServer::Initialize()
	{		
		Logger::Get()->LogThis("Game Server Starting...");
		
		serializationManager = std::make_shared<SerializationManager>(Encoding);
		eventManager = EventManager::Get();
		networking = Networking::Get();
		eventFactory = EventFactory::Get();
		
		gameServerConnection->Initialize();
		gameServerConnection->Create();
		
		// We're interested in some of the our own game's events
		eventManager->SubscribeToEvent(PlayerMovedEventTypeEventId, this);
		eventManager->SubscribeToEvent(ControllerMoveEventId, this);
		eventManager->SubscribeToEvent(StartNetworkLevelEventId, this);
	}
	
	void GameServer::Listen(const unsigned long deltaMs) const
	{		
		gameServerConnection->Listen(deltaMs);
	}
		
	void GameServer::CheckForPlayerTraffic(const unsigned long deltaMs) const
	{
		gameServerConnection->CheckForPlayerTraffic(deltaMs);		
	}

	void GameServer::Disconnect() const
	{
		gameServerConnection->Disconnect();
	}

	std::vector<std::shared_ptr<Event>> GameServer::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
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
