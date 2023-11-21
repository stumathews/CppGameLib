#pragma once
#include "IGameServerConnection.h"
#include <string>
#include <WinSock2.h>
#include <net/MessageHeader.h>
#include <vector>
#include <net/UdpNetworkPlayer.h>
#include <net/PeerInfo.h>
#include <events/EventSubscriber.h>
#include <events/Event.h>
namespace gamelib
{
	class Networking;
	class EventManager;
	class SerializationManager;
	class EventFactory;

	class UdpGameServerConnection final : public IGameServerConnection, public EventSubscriber
	{
	public:
		UdpGameServerConnection(const std::string& host, const std::string& port);
		// Inherited via IGameServerConnection
		void Initialize() override;
	private:
		SOCKET listeningSocket{};
		std::string host, port;

		SerializationManager* serializationManager;
		EventManager* eventManager;
		Networking* networking;
		EventFactory* eventFactory;

		// Inherited via IGameServerConnection
		void CheckForPlayerTraffic() override;
		void RaiseNetworkTrafficReceivedEvent(char buffer[512], int bytesReceived, PeerInfo fromClient) const;
		void SendToConnectedPlayersExceptToSender(const std::string& senderNickname, const std::string&
		                                          serializedMessage) const;
		timeval timeout{};
		fd_set readfds{};

		// Inherited via IGameServerConnection
		void Listen() override;

		// Inherited via IGameServerConnection
		void ProcessPingMessage(PeerInfo fromClient) const;
		void ProcessRequestPlayerDetailsMessage(const MessageHeader& messageHeader, const PeerInfo fromClient);
		void ParseReceivedPlayerPayload(const char* inPayload, int payloadLength, PeerInfo fromClient);
		std::vector<UdpNetworkPlayer> players;

		// Inherited via IGameServerConnection
		void SendEventToAllPlayers(std::string serializedEvent) override;

		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt, unsigned long deltaMs) override;
		std::string GetSubscriberName() override;

		// Inherited via IGameServerConnection
		void Create() override;

	public:
		void Disconnect() override;
	};
}

