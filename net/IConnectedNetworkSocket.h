#pragma once
#ifndef INETWORKSOCKET_H
#define INETWORKSOCKET_H
#include <net/Networking.h>

namespace gamelib
{
	// Interface to a client's connection to server
	class IConnectedNetworkSocket
	{
	public:
		virtual ~IConnectedNetworkSocket() = default;
		virtual void Connect(const char* address, const char* port) = 0;
		virtual bool IsTcp() = 0;
		virtual int Send(const char* data, int dataLength, unsigned long deltaMs = 0) = 0;
		virtual int Receive(char* readBuffer, int bufLength, unsigned long deltaMs = 0) = 0;
		[[nodiscard]] virtual SOCKET GetRawSocket() const = 0;
		virtual bool IsValid() = 0;
	};
}

#endif
