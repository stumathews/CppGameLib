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
		bool IsTcp() override {return true;}
		int Send(const char* data, const int dataLength) const override
		{
			return gamelib::Networking::netSendVRec(socket, data, dataLength);
		}
		int Receive(const char* readBuffer, const int bufLength) const override
		{
			return gamelib::Networking::netReadVRec(socket, const_cast<char*>(readBuffer), bufLength);
		}
		bool IsValid() override { return socket; }
		[[nodiscard]] SOCKET GetRawSocket() const override { return socket;}

	private:
		SOCKET socket = -1;
	};
}

#endif