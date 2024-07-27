#ifndef RELIABLE_GAME_SERVER_CONNECTION_H
#define RELIABLE_GAME_SERVER_CONNECTION_H
#pragma once

#include <string>
#include <WinSock2.h>
#include "UdpGameServerConnection.h"
#include "net/ReliableUdp.h"

namespace gamelib
{
	class Networking;
	class EventManager;
	class SerializationManager;
	class EventFactory;

	// Connection capable of packing and unpacking protocol data
	class ReliableUdpGameServerConnection final : public UdpGameServerConnection
	{
	public:
		ReliableUdpGameServerConnection(const std::string& host, const std::string& port);

	private:

		// When we check for player traffic, we need to unpack the received data
		void CheckForPlayerTraffic(unsigned long deltaMs) override;
		int InternalSend(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int toLen,
		                 unsigned long sendTimeMs, MessageType messageType);

		// When data is sent, we need to pack it up into the over-the-wire protocol format
		int InternalSend(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int toLen, unsigned long sendTimeMs = 0) override;
		int SendAck(const SOCKET socket, const Message& messageToAck, const int flags, PeerInfo& peerInfo, unsigned long sendTimeMs = 0);

		// Used to keep track of packets send/received
		ReliableUdp reliableUdp;
		bool sessionEstablished {false};
	};
}

#endif