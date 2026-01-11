#ifndef IPROTOCOL_MANAGER_H
#define IPROTOCOL_MANAGER_H
#pragma once
#include "Message.h"
#include <memory>

namespace gamelib
{
	class IGameClientConnection;

	// Interface for game client's networking protocol
	class IGameClientConnetionProtocolManager
	{
	public:
		virtual ~IGameClientConnetionProtocolManager() = default;

		// connect to server
		virtual void Connect(const char* address, const char* port) = 0;

		// send data to server
		virtual int Send(const char* data, int dataLength, unsigned long deltaMs = 0) = 0;

		// read data from server
		virtual int Receive(char* receivedBuffer, int bufLength, unsigned long deltaMs = 0) = 0;

		// send ack to server
		virtual int SendAck(const Message& messageToAck, unsigned long sendTimeMs) = 0;

		// initialize
		virtual bool Initialize() = 0;

		// Get client's socket to game server
		virtual std::shared_ptr<IGameClientConnection> GetConnection() = 0;
	};
}

#endif