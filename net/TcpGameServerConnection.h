#pragma once
#include "IGameServerConnection.h"
#include <string>
#include <WinSock2.h>
#include <vector>
#include <net/TcpNetworkPlayer.h>
#include <net/MessageHeader.h>
#include <events/Event.h>
#include <events/EventSubscriber.h>

namespace gamelib
{
	class Networking;
	class EventManager;
	class SerializationManager;
	class EventFactory;
	class TcpGameServerConnection : public IGameServerConnection,  public gamelib::EventSubscriber
	{
	public:

		TcpGameServerConnection(std::string host, std::string port);
		~TcpGameServerConnection();
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
		void ProcessPingMessage(const size_t& playerId);
		void SendToConnectedPlayersExceptToSender(const std::string& senderNickname, std::string serializedMessage, const size_t& playerId);
		void RaiseNetworkTrafficReceievedEvent(char buffer[512], const size_t& i, int bytesReceived);
		void ParseReceivedPlayerPayload(const size_t& playerId, char* inPayload, int payloadLength);
		void ProcessRequestPlayerDetailsMessage(int playerId, gamelib::MessageHeader& messageHeader);
		void SendEventToAllPlayers(std::string serializedEvent) override;
		fd_set readfds;

		// Inherited via IGameServerConnection
		virtual void Listen() override;
		void CheckForNewTcpPlayers();
		/// <summary>
		/// How long to wait for network data the arrive {0,0} means non-blocking
		/// </summary>
		struct timeval timeout;
		std::vector<gamelib::TcpNetworkPlayer> Players;

		// Inherited via EventSubscriber
		virtual std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt) override;
		virtual std::string GetSubscriberName() override;

		// Inherited via IGameServerConnection
		virtual void Create() override;
	};
}

