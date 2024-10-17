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

		// Mark a packet as being sent
		Message* MarkSent(PacketDatum datum, MessageType messageType);

		// Mark a packet as being received
		void MarkReceived(const Message& senderMessage, unsigned long receivedTimeMs = 0);

		// Generate 32-bit number that represents the last 32 packets received
		uint32_t GenerateAcknowledgedBits();

		// Store packets received in receive buffer
		RingBuffer<PacketDatum> ReceiveBuffer;

		// Store packets to be sent into the send buffer
		RingBuffer<PacketDatum> SendBuffer;

		// The last sequence number you received
		uint16_t LastAcknowledgedSequenceNumber {};
	private:

		// Keeps a record of the last sent message
		Message* sentMessage{};

		// Keeps track of the last sequence number generated
		uint16_t sequence {};

		const int maxMessageSizeBytes = 1200;
		// Limit the message size to a size that is below a good estimate for MTU to avoid sending packet being dropped
	};
}

#endif
