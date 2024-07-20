#pragma once
#ifndef RELIABLEUDPNETWORKSOCKET_H
#define RELIABLEUDPNETWORKSOCKET_H

#include <WinSock2.h>

#include "events/EventSubscriber.h"
#include "net/IConnectedNetworkSocket.h"
#include "net/ReliableUdp.h"



namespace gamelib
{
	class ReliableUdpNetworkSocket final : public EventSubscriber, public IConnectedNetworkSocket
	{
	public:
		explicit ReliableUdpNetworkSocket(SOCKET socket);
		ReliableUdpNetworkSocket();
		bool IsTcp() override;
		int Send(const char* callersSendBuffer, int dataLength) override;
		int Receive(char* callersReceiveBuffer, int bufLength) override;
		bool IsValid() override;
		[[nodiscard]] SOCKET GetRawSocket() const override;
		~ReliableUdpNetworkSocket() override = default;
		void Connect(const char* address, const char* port) override;
		std::string GetSubscriberName() override;
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs) override;
		int GetSubscriberId() override;
		

	private:
		SOCKET socket = -1;
		ReliableUdp reliableUdp;

		int SendAck(const Message& messageToAck);
		
		constexpr static auto PackingBufferElements = 300;
		constexpr static auto ReceiveBufferMaxElements = 300;
		uint32_t packingBuffer[PackingBufferElements]{};
		uint32_t readBuffer[ReceiveBufferMaxElements]{};
	};
}

#endif
