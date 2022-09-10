#pragma once
#include "NetworkPlayer.h"
#include <string>
#include <WinSock2.h>

namespace gamelib
{
	class TcpNetworkPlayer : public gamelib::NetworkPlayer
	{
	public:
		TcpNetworkPlayer()
		{
			this->socket = -1;
			this->isConnected = false;
		}

		TcpNetworkPlayer(SOCKET socket)
		{			
			this->socket = socket;
			this->isConnected = true;
		}			

		SOCKET GetSocket()
		{
			return this->socket;
		}

		void SetIsConnected(bool yesNo)
		{
			this->isConnected = yesNo;
		}

		bool IsConnected()
		{
			return this->isConnected;
		}

	private:
		SOCKET socket;
		bool isConnected;
	};
}

