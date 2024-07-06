#pragma once
#ifndef RELIABLEUDPNETWORKSOCKET_H
#define RELIABLEUDPNETWORKSOCKET_H

#include <WinSock2.h>
#include "net/IConnectedNetworkSocket.h"
#include "net/ReliableUdp.h"

constexpr auto PackingBufferElements = 8192;
constexpr auto ReceiveBufferMaxElements = 512;

namespace gamelib
{
	class ReliableUdpNetworkSocket final : public IConnectedNetworkSocket
	{
	public:
		explicit ReliableUdpNetworkSocket(const SOCKET socket);
		bool IsTcp() override;
		int Send(const char* callersSendBuffer, int dataLength) override;
		int Receive(const char* callersReceiveBuffer, int bufLength) override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~ReliableUdpNetworkSocket() override = default;

	private:
		SOCKET socket = -1;
		ReliableUdp reliableUdp;
		uint32_t packingBuffer[PackingBufferElements];
		char readBuffer[ReceiveBufferMaxElements];
	};
}

#endif
