#include "GameServer.h"
#include <events/EventManager.h>
#include <net/Networking.h>
#include <json/json11.h>
#include <events/EventFactory.h>
#include <file/SerializationManager.h>
#include <net/IGameServerConnection.h>
#include <net/TcpGameServerConnection.h>
#include <net/UdpGameServerConnection.h>
#include "events/PlayerMovedEvent.h"
#include "events/StartNetworkLevelEvent.h"
#include "file/Logger.h"
#include "utils/Utils.h"

using namespace json11;


namespace gamelib
{
	GameServer::GameServer(const std::shared_ptr<IGameServerConnection> &gameServerConnection,
	                       const std::string& nickName, const gamelib::Encoding wireFormat)
	{		
		this->gameServerConnection = gameServerConnection;
		this->nickname = nickName;
		this->encoding = wireFormat;
	}

	void GameServer::Initialize()
	{		
		Logger::Get()->LogThis("Game server initializing...");
		
		serializationManager = std::make_shared<SerializationManager>(encoding);
		eventManager = EventManager::Get();
		networking = Networking::Get();
		eventFactory = EventFactory::Get();
		
		gameServerConnection->Initialize();
		gameServerConnection->Create();
		
		// We're interested in some of our own game's events
		eventManager->SubscribeToEvent(PlayerMovedEventTypeEventId, this);
		eventManager->SubscribeToEvent(ControllerMoveEventId, this);
		eventManager->SubscribeToEvent(StartNetworkLevelEventId, this);

		Logger::Get()->LogThis("Game server initialization complete.");
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

	std::string GameServer::GetAddress() const
	{
		return gameServerConnection->GetAddress();
	}

	std::string GameServer::GetPort() const
	{
		return gameServerConnection->GetPort();
	}

	std::vector<std::shared_ptr<Event>> GameServer::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{
		gameServerConnection->SendEventToAllPlayers(serializationManager->SerializeEvent(evt, nickname)); 

		return {};
	}

	std::string GameServer::GetSubscriberName()
	{
		return "Game Server";
	}
		
	GameServer::~GameServer() = default;
}
