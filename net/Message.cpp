#include "Message.h"

gamelib::Message::Message(const uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits,
	const uint16_t n, const PacketDatum* inData)
{
	// Set the sequence for this message in the header
	Header.Sequence = sequence;
	Header.LastAckedSequence = lastAckedSequence;
	Header.LastAckedBits = previousAckedBits;
			
	for(auto i = 0; i < n; i++)
	{
		data.push_back(inData[i]);
	}
}

std::vector<gamelib::PacketDatum> gamelib::Message::Data() const
{
	return data;
}

uint16_t gamelib::Message::DataCount() const
{
	return data.size();
}

void gamelib::Message::Write(BitPacker<uint32_t>& bitPacker) const
{
	Header.Write(bitPacker);
}

void gamelib::Message::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	Header.Read(bitfieldReader);
}
