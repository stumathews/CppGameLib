#pragma once
#ifndef UDPNETWORKSOCKET_H
#define UDPNETWORKSOCKET_H

#include <net/Networking.h>
#include "IConnectedNetworkSocket.h"

namespace gamelib
{
	class UdpConnectedNetworkSocket final : public IConnectedNetworkSocket
	{
	public:
		explicit UdpConnectedNetworkSocket();

		void Connect(const char* address, const char* port) override;
		bool IsTcp() override;
		int Send(const char* data, int dataLength, unsigned long deltaMs = 0) override;
		int Receive(char* readBuffer, int bufLength, unsigned long deltaMs = 0) override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~UdpConnectedNetworkSocket() override = default;

	private:
		SOCKET socket = -1;
	};
}

#endif
