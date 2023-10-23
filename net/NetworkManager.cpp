#include "NetworkManager.h"
#include <file/SettingsManager.h>
#include <net/Networking.h>

namespace gamelib
{
	NetworkManager::NetworkManager()
	{
		isGameServer = SettingsManager::Get()->GetBool("networking", "isGameServer");		
		gameServerAddress = SettingsManager::Get()->GetString("networking", "gameServerAddress");
		gameServerPort = SettingsManager::Get()->GetString("networking", "gameServerPort");
		maxPlayers = SettingsManager::Get()->GetInt("networking", "maxPlayers");
	}

	bool NetworkManager::IsGameServer() const
	{
		return isGameServer;
	}

	bool NetworkManager::Initialize()
	{
		Networking::Get()->InitializeWinSock();
		
		Server = std::make_shared<GameServer>(gameServerAddress, gameServerPort);

		if(isGameServer)
		{			
			Server->Initialize();
		}
		else
		{
			Client = std::make_shared<GameClient>();
			Client->Initialize();
			Client->Connect(Server);
		}

		return true;
	}

	NetworkManager* NetworkManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new NetworkManager();
		}
		return Instance;
	}

	NetworkManager* NetworkManager::Instance = nullptr;

	
	NetworkManager::~NetworkManager()
	{
		
		Instance = nullptr;
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
				Client->Listen();
			}
		}
	}
}
