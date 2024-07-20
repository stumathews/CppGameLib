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

	class UdpGameServerConnection : public IGameServerConnection, public EventSubscriber
	{
	public:
		UdpGameServerConnection(const std::string& host, const std::string& port);
		// Inherited via IGameServerConnection
		void Initialize() override;
	protected:		
		SOCKET listeningSocket{};
		void RaiseNetworkTrafficReceivedEvent(const char buffer[], const int bytesReceived, const PeerInfo fromClient);
		void ParseReceivedPlayerPayload(const char* inPayload, int payloadLength, PeerInfo fromClient);
	private:
		std::string host, port;

		SerializationManager* serializationManager;
		EventManager* eventManager;
		Networking* networking;
		EventFactory* eventFactory;

		// Inherited via IGameServerConnection
		virtual void CheckForPlayerTraffic() override;
		
		virtual int InternalSend(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int tolen);
		void SendToConnectedPlayersExceptToSender(const std::string& senderNickname, const std::string&
		                                          serializedMessage);
		timeval timeout{};
		fd_set readfds{};

		// Inherited via IGameServerConnection
		void Listen() override;

		// Inherited via IGameServerConnection
		void ProcessPingMessage(PeerInfo fromClient);
		void ProcessRequestPlayerDetailsMessage(const MessageHeader& messageHeader, const PeerInfo fromClient);
		
		std::vector<UdpNetworkPlayer> players;

		// Inherited via IGameServerConnection
		void SendEventToAllPlayers(std::string serializedEvent) override;

		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs) override;
		std::string GetSubscriberName() override;

		// Inherited via IGameServerConnection
		void Create() override;
		
		
	protected:		
		constexpr static auto ReadBufferMaxElements = 300;
		constexpr static auto ReadBufferSizeInBytes = ReadBufferMaxElements * 32 / 8;
		uint32_t readBuffer[ReadBufferMaxElements]{};
	public:
		void Disconnect() override;
	};
}

