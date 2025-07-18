#pragma once
#ifndef MESSAGEHEADER_H
#define MESSAGEHEADER_H
#include <cstdint>

#include "net/BitfieldReader.h"
#include "net/BitPacker.h"

namespace gamelib
{
	enum MessageType : uint8_t
	{
		Ack = 0,
		SendPubKey,
		General
	};

	inline std::string MessageTypeToString(MessageType messageType)
	{
		if (messageType == Ack) return "Ack";
		if (messageType == General) return "General";
		if (messageType == SendPubKey) return "SendPubKey";

		return "Unknown";
	}

	struct ReliableUdpMessageHeader
	{
		 // Sequence number
	    uint16_t Sequence {};

		// sender's last sequence that was acknowledged by the receiver
	    uint16_t LastAckedSequence {};

		 // bit n is set means Sequence - n was acked ie the previous n sequence was acked
	    uint32_t LastAckedBits {};

		 // Message Types:
		 // 0 is acknowledgment,
		 // 1 RequestPublicKey
		 // 3 General
		MessageType MessageType {};

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