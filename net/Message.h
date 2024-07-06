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
		 */
		Message(uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits,
		                 uint16_t n, const std::vector<PacketDatum>& inData);

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
				
		bit_packing_types::String<uint32_t> FirstPacket;

	private:
		std::vector<PacketDatum> packets{};
	};
}
#endif
