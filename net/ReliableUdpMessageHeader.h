#pragma once
#ifndef MESSAGEHEADER_H
#define MESSAGEHEADER_H
#include <cstdint>

#include "net/BitfieldReader.h"
#include "net/BitPacker.h"

namespace gamelib
{
	struct ReliableUdpMessageHeader
	{
		 // Sequence number
	    uint16_t Sequence {};

		// sender's last sequence that was acknowledged by the receiver
	    uint16_t LastAckedSequence {};

		 // bit n is set means Sequence - n was acked ie the previous n sequence was acked
	    uint32_t LastAckedBits {};

		 // 0 is acknowledgment, positive numbers are message types
		uint8_t MessageType {};

		 // This is so that the server can throw away messages that have a different protocol Id
		uint16_t ProtocolId {};

		 // CRC checksym 
		uint32_t CheckSum {};
		
		
		 void Write(BitPacker<uint32_t>& bitPacker) const;
		 void Read(BitfieldReader<uint32_t>& bitfieldReader);
		
		 void Write(BitPacker<uint32_t>& bitPacker, bool includeChecksum) const;		
		 void Read(BitfieldReader<uint32_t>& bitfieldReader, const bool includeChecksum);
		 static int GetSizeInBits();
	};
}

#endif