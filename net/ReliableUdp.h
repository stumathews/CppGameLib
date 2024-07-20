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
		
		Message* MarkSent(PacketDatum datum, bool isAckMessage = false);
		void MarkReceived(const Message& senderMessage);

		uint32_t GeneratePreviousAckedBits();

		RingBuffer<PacketDatum> ReceiveBuffer;
		RingBuffer<PacketDatum> SendBuffer;	
		uint16_t lastAckedSequence {};
	private:
		Message* sentMessage{};
		uint16_t sequence {};

		const int maxMessageSizeBytes = 1200;
		// Limit the message size to a size that is below a good estimate for MTU to avoid sending packet being dropped
	};
}

#endif
