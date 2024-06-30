#pragma once
#ifndef UDPNETWORKSOCKET_H
#define UDPNETWORKSOCKET_H

#include <WinSock2.h>
#include "INetworkSocket.h"

namespace gamelib
{
	class UdpNetworkSocket final : public INetworkSocket
	{
	public:
		explicit UdpNetworkSocket(const SOCKET socket): socket(socket) {  }
		bool IsTcp() override;
		int Send(const char* data, int dataLength) const override;
		int Receive(const char* readBuffer, int bufLength) const override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~UdpNetworkSocket() override = default;

	private:
		SOCKET socket = -1;
	};
}

#endif
