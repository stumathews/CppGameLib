#ifndef RELIABLEGAMESERVERCONNECTION_H
#define RELIABLEGAMESERVERCONNECTION_H
#pragma once
#include <string>
#include <WinSock2.h>
#include <net/MessageHeader.h>
#include <vector>
#include <net/UdpNetworkPlayer.h>
#include <net/PeerInfo.h>
#include <events/EventSubscriber.h>
#include <events/Event.h>

#include "net/IGameServerConnection.h"
#include "net/ReliableUdp.h"

namespace gamelib
{
	class Networking;
	class EventManager;
	class SerializationManager;
	class EventFactory;

	class ReliableUdpGameServerConnection final : public IGameServerConnection, public EventSubscriber
	{
	public:
		ReliableUdpGameServerConnection(const std::string& host, const std::string& port);
		// Inherited via IGameServerConnection
		void Initialize() override;
	private:
		SOCKET listeningSocket{};
		std::string host, port;

		SerializationManager* serializationManager;
		EventManager* eventManager;
		Networking* networking;
		EventFactory* eventFactory;
		ReliableUdp reliableUdp;

		// Inherited via IGameServerConnection
		void CheckForPlayerTraffic() override;
		void RaiseNetworkTrafficReceivedEvent(char buffer[512], int bytesReceived, PeerInfo fromClient);
		int InternalSend(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int tolen);
		void SendToConnectedPlayersExceptToSender(const std::string& senderNickname, const std::string&
		                                          serializedMessage);
		timeval timeout{};
		fd_set readfds{};

		// Inherited via IGameServerConnection
		void Listen() override;

		// Inherited via IGameServerConnection
		void ProcessPingMessage(PeerInfo fromClient);
		void ProcessRequestPlayerDetailsMessage(const MessageHeader& messageHeader, const PeerInfo fromClient);
		void ParseReceivedPlayerPayload(const char* inPayload, int payloadLength, PeerInfo fromClient);
		std::vector<UdpNetworkPlayer> players;

		// Inherited via IGameServerConnection
		void SendEventToAllPlayers(std::string serializedEvent) override;

		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs) override;
		std::string GetSubscriberName() override;

		// Inherited via IGameServerConnection
		void Create() override;

	public:
		void Disconnect() override;
	};
}

#endif