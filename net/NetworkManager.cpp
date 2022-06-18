#include "NetworkManager.h"
#include <util/SettingsManager.h>

namespace gamelib
{
	NetworkManager::NetworkManager()
	{
		isGameServer = SettingsManager::Get()->GetBool("networking", "isGameServer");		
		gameServerAddress = SettingsManager::Get()->GetString("networking", "gameServerAddress");
		gameServerPort = SettingsManager::Get()->GetString("networking", "gameServerPort");
		maxPlayers = SettingsManager::Get()->GetInt("networking", "maxPlayers");
	}

	bool NetworkManager::Initialize()
	{
		// Initialize WinSock2
		Networking::Get()->Initialize();
		
		// We are either a game server or we are connecting to one
		Server = std::shared_ptr<gamelib::GameServer>(new gamelib::GameServer(gameServerAddress, gameServerPort));

		if(isGameServer)
		{			
			Server->Initialize();
		}
		else
		{
			Client = std::shared_ptr<gamelib::GameClient>(new gamelib::GameClient());
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

	NetworkEvent NetworkManager::GetNetworkEvent()
	{
		auto networking = Networking::Get();
		if(isGameServer)
		{			
			Server->Listen();
		}


		return NetworkEvent();
	}
}
