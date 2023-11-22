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
		
		Server = std::make_shared<GameServer>(gameServerAddress, gameServerPort, isTcp, nickName);

		if(isGameServer)
		{			
			Server->Initialize();
		}
		else
		{
			Client = std::make_shared<GameClient>(nickName, isTcp);
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
				Client->Listen();
			}
		}
	}
}
