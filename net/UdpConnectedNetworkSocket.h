#pragma once
#ifndef UDPNETWORKSOCKET_H
#define UDPNETWORKSOCKET_H

#include <WinSock2.h>
#include "IConnectedNetworkSocket.h"

namespace gamelib
{
	class UdpConnectedNetworkSocket final : public IConnectedNetworkSocket
	{
	public:
		explicit UdpConnectedNetworkSocket();

		void Connect(const char* address, const char* port) override;
		bool IsTcp() override;
		int Send(const char* data, int dataLength) override;
		int Receive(const char* readBuffer, int bufLength) override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~UdpConnectedNetworkSocket() override = default;

	private:
		SOCKET socket = -1;
	};
}

#endif
