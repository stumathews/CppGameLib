#ifndef RELIABLEGAMESERVERCONNECTION_H
#define RELIABLEGAMESERVERCONNECTION_H
#pragma once
#include <string>
#include <WinSock2.h>
#include "UdpGameServerConnection.h"
#include "net/ReliableUdp.h"

constexpr auto PackingBufferElements = 512;
constexpr auto ReceiveBufferMaxElements = 512;

namespace gamelib
{
	class Networking;
	class EventManager;
	class SerializationManager;
	class EventFactory;

	class ReliableUdpGameServerConnection final : public UdpGameServerConnection
	{
	public:
		ReliableUdpGameServerConnection(const std::string& host, const std::string& port);
	private:
	
		void CheckForPlayerTraffic() override;		
		int InternalSend(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int toLen) override;		

		ReliableUdp reliableUdp;
		uint32_t packingBuffer[PackingBufferElements]{};
		char receiveBuffer[ReceiveBufferMaxElements]{};
	};
}

#endif