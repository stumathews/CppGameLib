#include "PacketDatum.h"

gamelib::PacketDatum::PacketDatum()
{
	Acked = false;
}

gamelib::PacketDatum::PacketDatum(const bool acked, const char* customData, const unsigned long sendTimeMs): Acked(acked), dataString(customData), SendTime(sendTimeMs)
{
	
}