#include "NetworkManager.h"
#include <file/SettingsManager.h>
#include <net/Networking.h>

#include "GameServerConnectionFactory.h"
#include "NetworkConnectionFactory.h"

namespace gamelib
{
	NetworkManager::NetworkManager()
	{
		isGameServer = SettingsManager::Get()->GetBool("networking", "isGameServer");		
		gameServerAddress = SettingsManager::Get()->GetString("networking", "gameServerAddress");
		gameServerPort = SettingsManager::Get()->GetString("networking", "gameServerPort");
		maxPlayers = SettingsManager::Get()->GetInt("networking", "maxPlayers");

		nickName = SettingsManager::Get()->GetString("networking", "nickname");
		isTcp = SettingsManager::Get()->GetBool("networking", "isTcp");
	}

	bool NetworkManager::IsGameServer() const
	{
		return isGameServer;
	}

	bool NetworkManager::Initialize()
	{
		Networking::Get()->InitializeWinSock();

		auto connection = GameServerConnectionFactory::Create(isTcp, gameServerAddress, gameServerPort);
		Server = std::make_shared<GameServer>(gameServerAddress, gameServerPort, connection, nickName);

		if(isGameServer)
		{			
			Server->Initialize();
		}
		else
		{
			auto connection = NetworkConnectionFactory::Create(isTcp);
			Client = std::make_shared<GameClient>(nickName, connection);
			Client->Initialize();
			Client->Connect(Server);
		}

		return true;
	}

	NetworkManager* NetworkManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new NetworkManager();
		}
		return instance;
	}

	NetworkManager* NetworkManager::instance = nullptr;

	
	NetworkManager::~NetworkManager()
	{
		
		instance = nullptr;
	}

	void NetworkManager::PingGameServer() const
	{
		if(!isGameServer)
		{
			Client->PingGameServer();
		}
	}

	void NetworkManager::Listen() const
	{
		if (SettingsManager::Get()->GetBool("global", "isNetworkGame"))
		{
			if (isGameServer)
			{
				Server->Listen();
			}
			else
			{
				Client->Read();
			}
		}
	}
}
