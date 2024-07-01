#include "ReliableUdpMessageHeader.h"

void gamelib::ReliableUdpMessageHeader::Write(BitPacker<uint32_t>& bitPacker) const
{
	bitPacker.Pack(16, Sequence);
	bitPacker.Pack(16, LastAckedSequence);
	bitPacker.Pack(32, LastAckedBits);
}

void gamelib::ReliableUdpMessageHeader::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	Sequence = bitfieldReader.ReadNext<uint16_t>(16);
	LastAckedSequence = bitfieldReader.ReadNext<uint16_t>(16);
	LastAckedBits = bitfieldReader.ReadNext<uint32_t>(32);
}