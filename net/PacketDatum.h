#pragma once
#ifndef PACKETDATUM_H
#define PACKETDATUM_H

#include <cstdint>

#include "net/BitfieldReader.h"
#include "net/BitPacker.h"

namespace gamelib
{
	struct PacketDatum
	{
		PacketDatum();
		explicit PacketDatum(bool acked, const char* customData = "");

		void Write(BitPacker<uint32_t>& bitPacker) const;

		void Read(BitfieldReader<const char>& bitfieldReader);

		bool Acked;
		uint16_t Sequence {};
		double SendTime {};
		const char* data{};


	};
}
#endif
