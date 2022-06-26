#pragma once
#include "IGameServerConnection.h"
#include <string>
#include <WinSock2.h>
#include <net/MessageHeader.h>
#include <vector>
#include <net/UdpNetworkPlayer.h>
#include <net/PeerInfo.h>
namespace gamelib
{
	class Networking;
	class EventManager;
	class SerializationManager;
	class EventFactory;
	class UdpGameServerConnection : public IGameServerConnection
	{
	public:
		UdpGameServerConnection(std::string host, std::string port);
		// Inherited via IGameServerConnection
		virtual void Initialize() override;
	private:
		SOCKET listeningSocket;
		std::string host, port;

		SerializationManager* serializationManager;
		EventManager* eventManager;
		Networking* networking;
		EventFactory* eventFactory;

		// Inherited via IGameServerConnection
		virtual void CheckForPlayerTraffic() override;
		void RaiseNetworkTrafficReceievedEvent(char buffer[512], int bytesReceived, PeerInfo fromClient);
		void SendToConnectedPlayersExceptToSender(const std::string& senderNickname, std::string serializedMessage);
		struct timeval timeout;
		fd_set readfds;

		// Inherited via IGameServerConnection
		virtual void Listen() override;

		// Inherited via IGameServerConnection
		void ProcessPingMessage(PeerInfo fromClient);
		void ProcessRequestPlayerDetailsMessage(gamelib::MessageHeader& messageHeader, PeerInfo fromClient);
		void ParseReceivedPlayerPayload(char* inPayload, int payloadLength, PeerInfo fromClient);
		std::vector<UdpNetworkPlayer> Players;

		// Inherited via IGameServerConnection
		virtual void SendEventToAllPlayers(std::string serializedEvent) override;
	};
}

