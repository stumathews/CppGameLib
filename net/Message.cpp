#include "Message.h"
#include "crc.h"

gamelib::Message::Message(const uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits,
                          const uint16_t numberOfPackets, const std::vector<PacketDatum>& inPackets, const MessageType messageType)
{
	// Construct the message's header
	Header.Sequence = sequence;
	Header.LastAckedSequence = lastAckedSequence;
	Header.LastAckedBits = previousAckedBits;
	Header.ProtocolId = 12;
	Header.messageType = messageType;
	
	// Store list of previously unsent messages
	for(auto i = 0; i < numberOfPackets; i++)
	{
		packets.push_back(inPackets[i]);
	}
	
	// Generate checksum last as it includes all the packet data also
	Header.CheckSum = GenerateCheckSum();
}

// internal
void gamelib::Message::Write(BitPacker<uint32_t>& bitPacker, const bool includeCheckSum) const
{
	// Pack the header
	Header.Write(bitPacker, includeCheckSum);

	// Pack the number of packets to include in this message 
	bitPacker.Pack(16, static_cast<uint32_t>(packets.size()));

	// Pack all those packet right after the number of packets
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

	// Read the number of packets to unpack
	const auto arraySize = bitfieldReader.ReadNext<int>(16);

	// Unpack that many packets
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
	// Write the message to the provided bit-packer
	Write(bitPacker, true);	
}

// public
void gamelib::Message::Read(BitfieldReader<uint32_t>& bitfieldReader)
{
	// Read the message from the provided bitfieldReader
	Read(bitfieldReader, true);
}

uint32_t gamelib::Message::GenerateCheckSum()
{
	constexpr auto sizeInBytes = CheckSumBufferElements *  32 / 8; // 1200 bytes provided buffer is of type uint32_t

	memset(checkSumBuffer, 0, sizeInBytes);

	// Create a Bit-packer linked to the checksum buffer
	BitPacker packer(checkSumBuffer, CheckSumBufferElements);

	// Pack without the checksum
	Write(packer, false);	

	// Create CRC value from the checksum buffer
	Crc32 crc;
	return crc.compute(reinterpret_cast<uint8_t*>(checkSumBuffer), static_cast<size_t>(sizeInBytes));
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


