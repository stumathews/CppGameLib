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
	    uint16_t Sequence; // current sequence
	    uint16_t LastAckedSequence;
	    uint32_t LastAckedBits; // bit n is set means Sequence - n was acked ie the previous n sequence was acked

		void Write(BitPacker<uint32_t>& bitPacker) const;
	    void Read(BitfieldReader<uint32_t>& bitfieldReader);
	};
}

#endif