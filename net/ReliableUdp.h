#pragma once
#ifndef RELIABLEUDP_H
#define RELIABLEUDP_H
#include <cstdint>
#include <memory>

#include "Message.h"
#include "PacketDatum.h"
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
		Message* MarkSent(PacketDatum datum);
		void MarkReceived(const Message& message);

		uint32_t GeneratePreviousAckedBits();

		RingBuffer<PacketDatum> ReceiveBuffer;
		RingBuffer<PacketDatum> SendBuffer;	
		uint16_t lastAckedSequence {};
	private:
		Message* sentMessage{};
		uint16_t sequence {};	
	};
}

#endif
