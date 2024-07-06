﻿#pragma once
#ifndef INETWORKSOCKET_H
#define INETWORKSOCKET_H
#include <WinSock2.h>

namespace gamelib
{
	class IConnectedNetworkSocket
	{
	public:
		virtual ~IConnectedNetworkSocket() = default;
		virtual void Connect(const char* address, const char* port) = 0;
		virtual bool IsTcp() = 0;
		virtual int Send(const char* data, int dataLength) = 0;
		virtual int Receive(const char* readBuffer, int bufLength) = 0;
		[[nodiscard]] virtual SOCKET GetRawSocket() const = 0;
		virtual bool IsValid() = 0;
	};
}

#endif