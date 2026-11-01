#ifndef NO_PROTOCOL_MANAGER_H
#define NO_PROTOCOL_MANAGER_H
#include <memory>

#include "IGameClientConnection.h"
#include "IGameClientConnetionProtocolManager.h"

namespace gamelib
{
	class TransportOnlyProtocolManager : public IGameClientConnetionProtocolManager
	{
	public:
		TransportOnlyProtocolManager(std::shared_ptr<IGameClientConnection> gameClientConnection): gameClientConnection(std::move(gameClientConnection)), isGameServer(false)
		{
			
		}

		TransportOnlyProtocolManager(std::shared_ptr<IGameServerConnection> gameServerConnection): gameServerConnection(std::move(gameServerConnection)), isGameServer(true)
		{
			
		}

		void Connect(const char* address, const char* port) override
		{
			gameClientConnection->Connect(address, port);
		}
		int Send(const char* data, int dataLength, const cppgamelib::DeltaTime* deltaTime) override
		{
			return gameClientConnection->Send(data, dataLength, deltaTime);
		}
		int Receive(char* receivedBuffer, int bufLength, const cppgamelib::DeltaTime* deltaTime) override
		{
			return gameClientConnection->Receive(receivedBuffer, bufLength, deltaTime);
		}
		int SendAck(const Message& messageToAck, unsigned long sendTimeMs) override
		{
			return 0;
		}
		
		bool Initialize() override
		{
			return true;
		}
		std::shared_ptr<IGameClientConnection> GetConnection() override
		{
			return gameClientConnection;
		}

		~TransportOnlyProtocolManager() override
		{
			
		}

	private:
		std::shared_ptr<IGameClientConnection> gameClientConnection;
		std::shared_ptr<IGameServerConnection> gameServerConnection;
		bool isGameServer;
	};
}

#endif
#pragma once
