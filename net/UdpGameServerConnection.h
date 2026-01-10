#pragma once
#include "IGameServerConnection.h"
#include <string>
#include <net/Networking.h>
#include <vector>
#include <net/UdpNetworkPlayer.h>
#include <net/PeerInfo.h>
#include <events/EventSubscriber.h>
#include <events/Event.h>
#include "file/SerializationManager.h"
#include "security/Security.h"

namespace gamelib
{
	class Networking;
	class EventManager;
	class EventFactory;

	class UdpGameServerConnection : public IGameServerConnection, public EventSubscriber
	{
	public:
		UdpGameServerConnection(const std::string& host, const std::string& port, Encoding wireFormat);
		// Inherited via IGameServerConnection
		void Initialize() override;
	protected:		
		SOCKET listeningSocket{};
		Encoding TheEncoding;
		void RaiseNetworkTrafficReceivedEvent(const char* buffer, int bytesReceived, const PeerInfo &fromClient);
		void ParseReceivedPlayerPayload(const char* inPayload, int payloadLength, const PeerInfo &fromClient);
	private:
		std::string host, port;

		std::shared_ptr<SerializationManager> serializationManager;
		EventManager* eventManager;
		Networking* networking;
		EventFactory* eventFactory;

		// Inherited via IGameServerConnection
		void CheckForPlayerTraffic(unsigned long deltaMs) override;
		
		virtual int InternalSend(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int toLen, unsigned long sendTimeMs = 0);
		void SendToConnectedPlayersExceptToSender(const std::string& senderNickname, const std::string&
		                                          serializedMessage);
		timeval timeout{};
		fd_set readfds{};

		// Inherited via IGameServerConnection
		void Listen(unsigned long deltaMs) override;

		// Inherited via IGameServerConnection
		void ProcessPingMessage(PeerInfo fromClient);
		void ProcessRequestPlayerDetailsMessage(const MessageHeader& messageHeader, const PeerInfo &fromClient);
		
		std::vector<UdpNetworkPlayer> players;

		// Inherited via IGameServerConnection
		void SendEventToAllPlayers(std::string serializedEvent) override;

		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, unsigned long deltaMs) override;
		std::string GetSubscriberName() override;

		// Inherited via IGameServerConnection
		void Create() override;
		
		
	protected:		
		constexpr static auto ReadBufferMaxElements = 300;
		constexpr static auto ReadBufferSizeInBytes = ReadBufferMaxElements * 32 / 8;
		uint32_t readBuffer[ReadBufferMaxElements]{};
		
		SecuritySide securitySide;
		unsigned char remotePublicKey[SecuritySide::PublicKeyLengthBytes]{0};
		unsigned char sharedNonce[SecuritySide::NonceLengthBytes]{0};
	public:
		void Disconnect() override;
		std::string GetAddress() override;
		std::string GetPort() override;
	};
}

