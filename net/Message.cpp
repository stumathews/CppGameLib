#include "Message.h"

gamelib::Message::Message(const uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits,
	const uint16_t n, const std::vector<PacketDatum>& inData)
{
	// Set the sequence for this message in the header
	Header.Sequence = sequence;
	Header.LastAckedSequence = lastAckedSequence;
	Header.LastAckedBits = previousAckedBits;

	// Store list of previously unsent messages
	std::vector<std::string> text;
	for(auto i = 0; i < n; i++)
	{
		packets.push_back(inData[i]);
		text.emplace_back(inData[i].data);
	}

	// Currently we don't support message aggregation, only the current unsacked message is sent
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
	// Pack the header
	Header.Write(bitPacker);

	// Pack the first packet
	//FirstPacket.Write(bitPacker);
}

void gamelib::Message::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	// Unpack into bitField reader's associated buffer
	Header.Read(bitfieldReader);

	// Unpack into bitField's associated buffer
	//FirstPacket.Read(bitfieldReader);
}
