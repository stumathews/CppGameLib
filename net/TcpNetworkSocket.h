#pragma once
#ifndef TCPNETWORKSOCKET_H
#define TCPNETWORKSOCKET_H
#include "IGameClientConnection.h"
//#include "Networking.h"

namespace gamelib
{
	class TcpNetworkSocket final : public IGameClientConnection
	{
	public:
		explicit TcpNetworkSocket(): socket(0) {  }

		bool IsTcp() override;
		int Send(const char* data, int dataLength, unsigned long deltaMs) override;
		int Receive(char* readBuffer, int bufLength, unsigned long deltaMs) override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~TcpNetworkSocket() override = default;
		void Connect(const char* address, const char* port) override;

	private:
		SOCKET socket = -1;
	};
}

#endif