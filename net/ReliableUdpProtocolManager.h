#include <memory>
#include <xstring>

#include "IGameServerConnection.h"
#include "IProtocolManager.h"
#include "ai/FSM.h"
#include "events/EventSubscriber.h"
#include "net/BitPacker.h"
#include "net/IConnectedNetworkSocket.h"
#include "net/ReliableUdp.h"
#include "security/Security.h"
#include <time/PeriodicTimer.h>
#ifndef NETWORK_PROTOCOL_MANAGER_H
#define NETWORK_PROTOCOL_MANAGER_H
#pragma once

namespace gamelib
{
	

	class ReliableUdpProtocolManager : public IProtocolManager, public EventSubscriber
	{
		private:
			bool initialized{};

			/// <summary>
			/// The socket the game client will use to communicate with the game server
			/// </summary>
			std::shared_ptr<IConnectedNetworkSocket> gameClientConnection;
			SecuritySide clientSecuritySide;
			std::shared_ptr<IGameServerConnection> gameServerConnection;
			constexpr static auto PackingBufferElements = 300;
			constexpr static auto ReceiveBufferMaxElements = 300;
			uint32_t packingBuffer[PackingBufferElements]{};
			uint32_t readBuffer[ReceiveBufferMaxElements]{};
			unsigned char remotePublicKey[SecuritySide::PublicKeyLengthBytes]{0};
			unsigned char sharedNonce[SecuritySide::NonceLengthBytes]{0};
			ReliableUdp reliableUdp;

			bool sessionEstablished {false};
			int SendInternal(const char* callersSendBuffer, int dataLength, unsigned long deltaMs, MessageType messageType);
			int ReceiveInternal(char* callersReceiveBuffer, int bufLength, unsigned long deltaMs);
		

		public:
			ReliableUdpProtocolManager(std::shared_ptr<IConnectedNetworkSocket> gameClientConnection);
			ReliableUdpProtocolManager(std::shared_ptr<IGameServerConnection> gameServerConnection);

			// Cannot copy
			ReliableUdpProtocolManager(ReliableUdpProtocolManager const&) = delete;

			// Cannot assign to
			void operator=(ReliableUdpProtocolManager const&) = delete;
		
			bool Initialize() override;		
			std::shared_ptr<IConnectedNetworkSocket> GetConnection() override;
			void Connect(const char* address, const char* port) const override;

			int Send(const char* callersSendBuffer, int dataLength, unsigned long deltaMs = 0) override;
			int Receive(char* callersReceiveBuffer, int bufLength, unsigned long deltaMs = 0) override;
			int SendAck(const Message& messageToAck, unsigned long sendTimeMs) override;
			void Update(unsigned long deltaMs);
			std::string GetSubscriberName() override;
			std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, unsigned long deltaMs) override;
			~ReliableUdpProtocolManager() override;
			int GetSubscriberId() override;
	};

}

#endif

