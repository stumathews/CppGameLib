#pragma once
#ifndef RELIABLEUDP_H
#define RELIABLEUDP_H
#include <cstdint>
#include <memory>
#include <vector>

#include "Message.h"
#include "Option.h"
#include "PacketDatum.h"
#include "UdpNetworkSocket.h"
#include "Tests/Tests/TestData.h"
#include "utils/BitFiddler.h"
#include "utils/RingBuffer.h"

namespace gamelib
{
	class GameClient;

	/*

		sequence
		lastAckedSequence
		lastAckedBits
		payloadSizeBytes
		payload
	 
	 */

	class ReliableUdp
	{
	public:		

		Message* Send(std::shared_ptr<GameClient> client, PacketDatum datum);
		int Send(const std::shared_ptr<INetworkSocket>& networkSocket, const PacketDatum datum);
		static libmonad::Option<gamelib::Message> Receive(const std::shared_ptr<INetworkSocket>& networkSocket);
		Message* MarkSent(PacketDatum datum);
		void MarkReceived(const Message& message);

		uint32_t GeneratePreviousAckedBits();

		RingBuffer<PacketDatum> receiveBuffer;
		RingBuffer<PacketDatum> sendBuffer;	
		uint16_t lastAckedSequence {};
	private:
		Message* sentMessage{};
		uint16_t sequence {};	
	};
}

#endif
