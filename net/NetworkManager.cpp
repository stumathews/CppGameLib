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
		const auto useReliableUdp = SettingsManager::Get()->GetBool("networking", "useReliableUdp");
		auto gameServerConnection = GameServerConnectionFactory::Create(isTcp, gameServerAddress, gameServerPort, useReliableUdp);
		Server = std::make_shared<GameServer>(gameServerAddress, gameServerPort, gameServerConnection, nickName);

		if(isGameServer)
		{			
			Server->Initialize();
		}
		else
		{
			Client = std::make_shared<GameClient>(nickName, NetworkConnectionFactory::Create(isTcp), useReliableUdp);
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

	void NetworkManager::PingGameServer(const unsigned long deltaMs) const
	{
		if(!isGameServer)
		{
			Client->PingGameServer(deltaMs);
		}
	}

	void NetworkManager::Listen(const unsigned long deltaMs) const
	{
		if (SettingsManager::Get()->GetBool("global", "isNetworkGame"))
		{
			if (isGameServer)
			{
				Server->Listen(deltaMs);
			}
			else
			{
				Client->Read(deltaMs);
			}
		}
	}
}
