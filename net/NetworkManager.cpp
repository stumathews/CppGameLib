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

	bool NetworkManager::Initialize()
	{
		Networking::Get()->Initialize();
		
		// We are either a game server or we are connecting to one
		Server = std::shared_ptr<gamelib::GameServer>(new gamelib::GameServer(gameServerAddress, gameServerPort));

		if(isGameServer)
		{			
			Server->Initialize();
			// We also want to connect to ourselves as a client, so that incomming player data is sent to 'all connected' clients
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

	NetworkEvent NetworkManager::GetNetworkEvent()
	{
		auto networking = Networking::Get();
		if(isGameServer)
		{			
			// Check for any incomming traffic
			Server->Listen();
		}else
		{
			// Check for any incomming traffic 
			Client->Listen();
		}


		return NetworkEvent();
	}
}
