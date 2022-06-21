#pragma once
#include <string>
#include <WinSock2.h>
namespace gamelib
{
	class NetworkPlayer
	{
	public:
		NetworkPlayer()
		{
			this->socket = -1;
			this->isConnected = false;
		}

		NetworkPlayer(SOCKET socket)
		{			
			this->socket = socket;
			this->isConnected = true;
		}

		void SetNickName(const std::string nick)
		{
			this->NickName = nick;
		}

		std::string GetNickName()
		{
			return this->NickName;
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
		std::string NickName;
		SOCKET socket;
		bool isConnected;
	};
}

