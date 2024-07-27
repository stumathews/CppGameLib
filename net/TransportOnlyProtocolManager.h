#ifndef NO_PROTOCOL_MANAGER_H
#define NO_PROTOCOL_MANAGER_H
#include <memory>

#include "IGameServerConnection.h"
#include "IProtocolManager.h"
#include "Message.h"

namespace gamelib
{
	class IConnectedNetworkSocket;

	class TransportOnlyProtocolManager : public IProtocolManager
	{
	public:
		TransportOnlyProtocolManager(std::shared_ptr<IConnectedNetworkSocket> gameClientConnection): gameClientConnection(std::move(gameClientConnection)), isGameServer(false)
		{
			
		}

		TransportOnlyProtocolManager(std::shared_ptr<IGameServerConnection> gameServerConnection): gameServerConnection(std::move(gameServerConnection)), isGameServer(true)
		{
			
		}

		void Connect(const char* address, const char* port) const override
		{
			gameClientConnection->Connect(address, port);
		}
		int Send(const char* data, int dataLength, unsigned long deltaMs) override
		{
			return gameClientConnection->Send(data, dataLength, deltaMs);
		}
		int Receive(char* receivedBuffer, int bufLength, unsigned long deltaMs) override
		{
			return gameClientConnection->Receive(receivedBuffer, bufLength, deltaMs);
		}
		int SendAck(const Message& messageToAck, unsigned long sendTimeMs) override
		{
			return 0;
		}
		
		bool Initialize() override
		{
			return true;
		}
		std::shared_ptr<IConnectedNetworkSocket> GetConnection() override
		{
			return gameClientConnection;
		}

		~TransportOnlyProtocolManager() override
		{
			
		}

	private:
		std::shared_ptr<IConnectedNetworkSocket> gameClientConnection;
		std::shared_ptr<IGameServerConnection> gameServerConnection;
		bool isGameServer;
	};
}

#endif
#pragma once
