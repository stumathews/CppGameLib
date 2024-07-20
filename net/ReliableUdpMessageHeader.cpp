#include "ReliableUdpMessageHeader.h"

void gamelib::ReliableUdpMessageHeader::Write(BitPacker<uint32_t>& bitPacker) const
{
	bitPacker.Pack(16, Sequence);
	bitPacker.Pack(16, LastAckedSequence);
	bitPacker.Pack(32, LastAckedBits);
	bitPacker.Pack(BITS_REQUIRED(0,1), MessageType);
}

void gamelib::ReliableUdpMessageHeader::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	Sequence = bitfieldReader.ReadNext<uint16_t>(16);
	LastAckedSequence = bitfieldReader.ReadNext<uint16_t>(16);
	LastAckedBits = bitfieldReader.ReadNext<uint32_t>(32);
	MessageType = bitfieldReader.ReadNext<uint32_t>(BITS_REQUIRED(0,1));
}
