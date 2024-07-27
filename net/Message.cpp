#include "Message.h"

#include "crc.h"

gamelib::Message::Message(const uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits,
                          const uint16_t n, const std::vector<PacketDatum>& inData, const MessageType messageType)
{
	// Set the sequence for this message in the header
	Header.Sequence = sequence;
	Header.LastAckedSequence = lastAckedSequence;
	Header.LastAckedBits = previousAckedBits;
	Header.ProtocolId = 12;
	Header.MessageType = messageType;
	
	// Store list of previously unsent messages
	for(auto i = 0; i < n; i++)
	{
		packets.push_back(inData[i]);
	}
	
	// Generate checksum last as it includes all the packet data also
	Header.CheckSum = GenerateCheckSum();
}

uint32_t gamelib::Message::GenerateCheckSum()
{
	constexpr auto sizeInBytes = CheckSumBufferElements *  32 / 8; // 1200 bytes provided buffer is of type uint32_t

	memset(checkSumBuffer, 0, sizeInBytes);

	BitPacker packer(checkSumBuffer, CheckSumBufferElements);

	Write(packer, false);	
	
	return Crc32::crc32buf(reinterpret_cast<const char*>(checkSumBuffer), static_cast<size_t>(sizeInBytes));
}

bool gamelib::Message::ValidateChecksum(const uint32_t yourChecksum)
{
	const auto myChecksum = GenerateCheckSum();
	return myChecksum == yourChecksum;
}

bool gamelib::Message::ValidateChecksum()
{
	return ValidateChecksum(this->Header.CheckSum);
}

std::vector<gamelib::PacketDatum> gamelib::Message::Data() const
{
	return packets;
}

uint16_t gamelib::Message::DataCount() const
{
	return packets.size();
}

// internal
void gamelib::Message::Write(BitPacker<uint32_t>& bitPacker, const bool includeCheckSum) const
{
	// Pack the header
	Header.Write(bitPacker, includeCheckSum);

	bitPacker.Pack(16, packets.size());

	for (const auto& packetDatum : packets)
	{
		packetDatum.Write(bitPacker);
	}	
}

// internal
void gamelib::Message::Read(BitfieldReader<uint32_t>& bitfieldReader, const bool includeCheckSum)
{
	// Unpack into bitField reader's associated buffer
	Header.Read(bitfieldReader, includeCheckSum);

	const auto arraySize = bitfieldReader.ReadNext<int>(16);
	for(int i = 0 ; i < arraySize; i++)
	{
		PacketDatum temp;
		temp.Read(bitfieldReader);
		packets.push_back(temp);
	}
}

// public
void gamelib::Message::Write(BitPacker<uint32_t>& bitPacker) const
{
	Write(bitPacker, true);	
}

// public
void gamelib::Message::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	Read(bitfieldReader, true);
}
