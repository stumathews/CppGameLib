#include "Message.h"

gamelib::Message::Message(const uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits,
	const uint16_t n, const std::vector<PacketDatum>& inData)
{
	// Set the sequence for this message in the header
	Header.Sequence = sequence;
	Header.LastAckedSequence = lastAckedSequence;
	Header.LastAckedBits = previousAckedBits;

	// Store list of previously unsent messages
	for(auto i = 0; i < n; i++)
	{
		packets.push_back(inData[i]);
	}
}

std::vector<gamelib::PacketDatum> gamelib::Message::Data() const
{
	return packets;
}

uint16_t gamelib::Message::DataCount() const
{
	return packets.size();
}

void gamelib::Message::Write(BitPacker<uint32_t>& bitPacker) const
{
	// Pack the header
	Header.Write(bitPacker);

	bitPacker.Pack(16, packets.size());

	for (const auto& packetDatum : packets)
	{
		packetDatum.Write(bitPacker);
	}	
}

void gamelib::Message::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	// Unpack into bitField reader's associated buffer
	Header.Read(bitfieldReader);

	const auto arraySize = bitfieldReader.ReadNext<int>(16);
	for(int i = 0 ; i < arraySize; i++)
	{
		PacketDatum temp;
		temp.Read(bitfieldReader);
		packets.push_back(temp);
	}
}
