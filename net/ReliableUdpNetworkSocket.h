#pragma once
#ifndef RELIABLEUDPNETWORKSOCKET_H
#define RELIABLEUDPNETWORKSOCKET_H

#include <WinSock2.h>
#include "net/INetworkSocket.h"
#include "net/ReliableUdp.h"

namespace gamelib
{
	class ReliableUdpNetworkSocket final : public INetworkSocket
	{
	public:
		explicit ReliableUdpNetworkSocket(const SOCKET socket);
		bool IsTcp() override;
		int Send(const char* data, int dataLength) override;
		int Receive(const char* readBuffer, int bufLength) override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~ReliableUdpNetworkSocket() override = default;

	private:
		SOCKET socket = -1;
		ReliableUdp reliableUdp;
	};
}

#endif
