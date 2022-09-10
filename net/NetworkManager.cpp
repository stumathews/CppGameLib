#include "NetworkManager.h"
#include <util/SettingsManager.h>
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

	bool NetworkManager::IsGameServer()
	{
		return isGameServer;
	}

	bool NetworkManager::Initialize()
	{
		Networking::Get()->InitializeWinSock();
		
		Server = std::shared_ptr<gamelib::GameServer>(new gamelib::GameServer(gameServerAddress, gameServerPort));

		if(isGameServer)
		{			
			Server->Initialize();
		}
		else
		{
			Client = std::shared_ptr<gamelib::GameClient>(new gamelib::GameClient());
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

	void NetworkManager::Listen()
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
