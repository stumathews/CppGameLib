#pragma once
#include "GameServer.h"
#include <net/Networking.h>
#include <common/Logger.h>

namespace gamelib
{
	class GameClient
	{
	public:
		void Connect(std::shared_ptr<GameServer> gameServer);
	private:
		std::shared_ptr<GameServer> gameServer;
		SOCKET clientSocket;
	};
}

