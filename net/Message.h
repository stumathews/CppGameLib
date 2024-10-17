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
		 * \param numberOfPackets number of contained packets (if > 1 this is an aggregated message containing other messages)
		 * \param inPackets the contained packets
		 * \param messageType
		 */
		Message(uint16_t sequence, uint16_t lastAckedSequence, uint32_t previousAckedBits, uint16_t numberOfPackets,
		        const std::vector<PacketDatum>& inPackets, MessageType messageType);
		
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

		// Generate a checksum from the message
		uint32_t GenerateCheckSum();

		// Validate a specific checksum against the message
		bool ValidateChecksum(uint32_t yourChecksum);

		// validate the current checksum against the current message
		bool ValidateChecksum();
			
		
	private:

		// Read/Write Message to a buffer
		void Write(BitPacker<uint32_t>& bitPacker, bool includeCheckSum) const;
		void Read(BitfieldReader<uint32_t>& bitfieldReader, bool includeCheckSum);

		// list of contained packets (usually previously unacknowledged packets)
		std::vector<PacketDatum> packets{};

		// Checksum buffer
		static constexpr int CheckSumBufferElements = 300;
		uint32_t checkSumBuffer[CheckSumBufferElements] {0}; //enough to hold up to 1200 bytes of packet data for check-summing
	};
}
#endif
