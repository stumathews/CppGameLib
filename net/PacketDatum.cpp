#include "PacketDatum.h"

gamelib::PacketDatum::PacketDatum()
{
	Acked = false;
}

gamelib::PacketDatum::PacketDatum(const bool acked, const char* customData): Acked(acked), dataString(customData)
{
	
}