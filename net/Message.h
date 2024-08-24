#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H
#include <vector>

#include "BitPackingTypes.h"
#include "ReliableUdpMessageHeader.h"
#include "PacketDatum.h"

namespace gamelib
{
	class Message
	{
	public:

		/**
		 * \brief Creates a new network packet
		 * \param sequence consecutive sequence number
		 * \param lastAckedSequence last sequence number that was acknowledged
		 * \param previousAckedBits 
		 * \param n number of contained packets (if > 1 this is an aggregated message containing other messages)
		 * \param inData the contained packets
		 * \param isAckMessage is the message type is an ack type
		 */
		Message(uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits, uint16_t n, const std::vector<PacketDatum >& inData, MessageType messageType);
		uint32_t GenerateCheckSum();
		bool ValidateChecksum(uint32_t yourChecksum);
		bool ValidateChecksum();

		Message() = default;

		// Contains sequence number and list of previously acked messages encoded in 32-bit field
		ReliableUdpMessageHeader Header {};

		// Aggregate messages that were not previously acknowledged
		[[nodiscard]] std::vector<PacketDatum> Data() const;

		// Number of aggregate messages
		[[nodiscard]] uint16_t DataCount() const;
		

		// Pack into bitPacker's associated buffer
		void Write(BitPacker<uint32_t>& bitPacker) const;

		// Unpack into bitfieldReader's associated buffer
		void Read(BitfieldReader<uint32_t>& bitfieldReader);
			
		
	private:
		void Write(BitPacker<uint32_t>& bitPacker, bool includeCheckSum) const;
		void Read(BitfieldReader<uint32_t>& bitfieldReader, bool includeCheckSum);
		std::vector<PacketDatum> packets{};
		static constexpr int CheckSumBufferElements = 300;
		uint32_t checkSumBuffer[CheckSumBufferElements] {0}; //enough to hold up to 1200 bytes of packet data for check-summing
	};
}
#endif
