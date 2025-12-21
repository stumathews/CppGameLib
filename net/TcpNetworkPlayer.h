#pragma once
#include "NetworkPlayer.h"
#include <net/Networking.h>

namespace gamelib
{
	class TcpNetworkPlayer : public NetworkPlayer
	{
	public:
		TcpNetworkPlayer()
		{
			this->socket = -1;
			this->isConnected = false;
		}

		TcpNetworkPlayer(const SOCKET socket)
		{			
			this->socket = socket;
			this->isConnected = true;
		}

		[[nodiscard]] SOCKET GetSocket() const
		{
			return this->socket;
		}

		void SetIsConnected(const bool yesNo)
		{
			this->isConnected = yesNo;
		}

		[[nodiscard]] bool IsConnected() const
		{
			return this->isConnected;
		}

	private:
		SOCKET socket;
		bool isConnected;
	};
}

