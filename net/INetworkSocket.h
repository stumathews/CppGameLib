#pragma once
#ifndef INETWORKSOCKET_H
#define INETWORKSOCKET_H
#include <WinSock2.h>

namespace gamelib
{
	class INetworkSocket
	{
	public:
		virtual ~INetworkSocket() = default;
		virtual bool IsTcp() = 0;
		virtual int Send(const char* data, int dataLength) const = 0;
		virtual int Receive(const char* readBuffer, int bufLength) const = 0;
		[[nodiscard]] virtual SOCKET GetRawSocket() const = 0;
		virtual bool IsValid() = 0;
	};
}

#endif