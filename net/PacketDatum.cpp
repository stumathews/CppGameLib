#include "PacketDatum.h"

gamelib::PacketDatum::PacketDatum()
{
	IsAcked = false;
	IsDataPacked = true; // by default we'll assume that the messages are unpacked raw strings
}

gamelib::PacketDatum::PacketDatum(const bool isAcknowledged, const char* customData, const unsigned long sendTimeMs, const bool isString)
	: IsAcked(isAcknowledged), SendTimeMs(sendTimeMs), IsDataPacked(isString), dataString(customData)
{
	
}
