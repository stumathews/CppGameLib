#include "Message.h"

gamelib::Message::Message(const uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits,
	const uint16_t n, const std::vector<PacketDatum>& inData)
{
	// Set the sequence for this message in the header
	Header.Sequence = sequence;
	Header.LastAckedSequence = lastAckedSequence;
	Header.LastAckedBits = previousAckedBits;

	std::vector<std::string> text;
	for(auto i = 0; i < n; i++)
	{
		packets.push_back(inData[i]);
		text.push_back(inData[i].data);
	}
	
	FirstPacket.Initialize(text[0]);
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
	Header.Write(bitPacker);
	FirstPacket.Write(bitPacker);
}

void gamelib::Message::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	Header.Read(bitfieldReader);
	FirstPacket.Read(bitfieldReader);
}
