#ifndef IPROTOCOL_MANAGER_H
#define IPROTOCOL_MANAGER_H
#pragma once
#include "Message.h"
#include <memory>

namespace gamelib
{
	class IConnectedNetworkSocket;

	class IProtocolManager
	{
	public:
		virtual ~IProtocolManager() = default;
		virtual void Connect(const char* address, const char* port) = 0;
		virtual int Send(const char* data, int dataLength, unsigned long deltaMs = 0) = 0;
		virtual int Receive(char* receivedBuffer, int bufLength, unsigned long deltaMs = 0) = 0;
		virtual int SendAck(const Message& messageToAck, unsigned long sendTimeMs) = 0;
		virtual bool Initialize() = 0;		
		virtual std::shared_ptr<IConnectedNetworkSocket> GetConnection() = 0;
	};
}

#endif