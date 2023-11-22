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
	GameServer::GameServer(const std::string& address, const std::string& port, const bool useTcp = true, const std::string& nickName )
	{
		this->Address = address;
		this->Port = port;			
		this->isTcp = useTcp;
		this->gameServerConnection = nullptr;
		this->nickname = nickName;
	}

	void GameServer::Initialize()
	{		
		Logger::Get()->LogThis("Game Server Starting...");
		
		serializationManager = SerializationManager::Get();
		eventManager = EventManager::Get();
		networking = Networking::Get();
		eventFactory = EventFactory::Get();

		Logger::Get()->LogThis(this->isTcp ? "Using TCP" : "Using UDP");

		// Create a new server socket connection, either TCP or UDP depending on game configuration
		gameServerConnection = this->isTcp ? To<IGameServerConnection>(std::make_shared<TcpGameServerConnection>(Address, Port))
										   : To<IGameServerConnection>(std::make_shared<UdpGameServerConnection>(Address, Port));
		
		gameServerConnection->Initialize();
		gameServerConnection->Create();
		
		// We're interested in some of the our own game's events
		eventManager->SubscribeToEvent(PlayerMovedEventTypeEventId, this);
		eventManager->SubscribeToEvent(ControllerMoveEventId, this);
		eventManager->SubscribeToEvent(StartNetworkLevelEventId, this);
	}
	
	void GameServer::Listen() const
	{		
		gameServerConnection->Listen();
	}
		
	void GameServer::CheckForPlayerTraffic() const
	{
		gameServerConnection->CheckForPlayerTraffic();		
	}

	void GameServer::Disconnect() const
	{
		gameServerConnection->Disconnect();
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
