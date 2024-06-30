#include "PacketDatum.h"

gamelib::PacketDatum::PacketDatum()
{
	Acked = false;
}

gamelib::PacketDatum::PacketDatum(const bool acked, const char* customData): Acked(acked), data(customData)
{
	
}

void gamelib::PacketDatum::Write(gamelib::BitPacker<uint32_t>& bitPacker) const
{
	bitPacker.Pack(BITS_REQUIRED( 0, 1), Acked);
}

void gamelib::PacketDatum::Read(BitfieldReader<const char>& bitfieldReader)
{
	Acked = bitfieldReader.ReadNext<uint16_t>(16);
}
