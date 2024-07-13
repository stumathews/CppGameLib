#pragma once
#ifndef RELIABLEUDPNETWORKSOCKET_H
#define RELIABLEUDPNETWORKSOCKET_H

#include <WinSock2.h>
#include "net/IConnectedNetworkSocket.h"
#include "net/ReliableUdp.h"



namespace gamelib
{
	class ReliableUdpNetworkSocket final : public IConnectedNetworkSocket
	{
	public:
		explicit ReliableUdpNetworkSocket(const SOCKET socket);
		ReliableUdpNetworkSocket();
		bool IsTcp() override;
		int Send(const char* callersSendBuffer, int dataLength) override;
		int Receive(char* callersReceiveBuffer, int bufLength) override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~ReliableUdpNetworkSocket() override = default;
		void Connect(const char* address, const char* port) override;

	private:
		constexpr static auto PackingBufferElements = 8192;
		constexpr static auto ReceiveBufferMaxElements = 512;
		SOCKET socket = -1;
		ReliableUdp reliableUdp;
		uint32_t packingBuffer[PackingBufferElements]{};
		uint32_t readBuffer[ReceiveBufferMaxElements]{};
	};
}

#endif
