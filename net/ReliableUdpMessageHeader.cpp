#include "ReliableUdpMessageHeader.h"

void gamelib::ReliableUdpMessageHeader::Write(BitPacker<uint32_t>& bitPacker, const bool includeChecksum = true) const
{
	bitPacker.Pack(16, Sequence);
	bitPacker.Pack(16, LastAckedSequence);
	bitPacker.Pack(32, LastAckedBits);
	bitPacker.Pack(BITS_REQUIRED(0,1), MessageType);
	bitPacker.Pack(16, ProtocolId);
	if(includeChecksum)
	{
		bitPacker.Pack(32, CheckSum);
	}
	bitPacker.Finish(); //flush align to next boundary
}

void gamelib::ReliableUdpMessageHeader::Write(BitPacker<uint32_t>& bitPacker) const
{
	Write(bitPacker, true);
}

void gamelib::ReliableUdpMessageHeader::Read(BitfieldReader<uint32_t>& bitfieldReader, const bool includeChecksum = true)
{
	Sequence = bitfieldReader.ReadNext<uint16_t>(16);
	LastAckedSequence = bitfieldReader.ReadNext<uint16_t>(16);
	LastAckedBits = bitfieldReader.ReadNext<uint32_t>(32);
	MessageType = bitfieldReader.ReadNext<uint8_t>(BITS_REQUIRED(0,1));
	ProtocolId = bitfieldReader.ReadNext<uint16_t>(16);	
	if(includeChecksum)
	{
		CheckSum = bitfieldReader.ReadNext<uint32_t>(32);
	}
	bitfieldReader.Finish(); // align to next boundary
}

void gamelib::ReliableUdpMessageHeader::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	Read(bitfieldReader, true);
}
int gamelib::ReliableUdpMessageHeader::GetSizeInBits()
{
	// LastAckedSequence, LastAckedSequence, LastAckedBits
	constexpr auto size1 =  (sizeof LastAckedSequence + sizeof LastAckedSequence + sizeof LastAckedBits) * 8;

	// MessageTypes
	constexpr auto size2 = 1;

	// Protocol Id
	constexpr auto size3 = 16;

	//Checksum
	constexpr auto size4 = 32;

	return size1 + size2 + size3 + size4;
}
