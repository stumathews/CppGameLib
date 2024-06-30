#pragma once
#ifndef TCPNETWORKSOCKET_H
#define TCPNETWORKSOCKET_H
#include "INetworkSocket.h"
#include "Networking.h"

namespace gamelib
{

	class TcpNetworkSocket final : public INetworkSocket
	{
	public:
		explicit TcpNetworkSocket(const SOCKET socket): socket(socket) {  }
		bool IsTcp() override;
		int Send(const char* data, int dataLength) override;
		int Receive(const char* readBuffer, int bufLength) override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~TcpNetworkSocket() override =default;

	private:
		SOCKET socket = -1;
	};
}

#endif