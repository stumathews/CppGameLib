#pragma once
#ifndef PACKETDATUM_H
#define PACKETDATUM_H

#include <cstdint>

#include "BitPackingTypes.h"
#include "net/BitfieldReader.h"
#include "net/BitPacker.h"

namespace gamelib
{
	struct PacketDatum
	{	
		PacketDatum();
		explicit PacketDatum(bool acked, const char* customData = "");

		template <typename T>		
		void Write(BitPacker<T>& bitPacker) const
		{
			bitPacker.Pack(BITS_REQUIRED( 0, 1), Acked);
			bitPacker.Pack(16, Sequence);
			bit_packing_types::String<T>(dataString).Write(bitPacker);
		}

		template <typename T>
		void Read(BitfieldReader<T>& bitfieldReader)
		{
			Acked = bitfieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0, 1));
			Sequence = bitfieldReader.ReadNext<uint16_t>(16);
			bit_packing_types::String<T> temp;
			temp.Read(bitfieldReader);
			dataString = temp.c_str();
		}

		bool Acked;
		uint16_t Sequence {};
		[[nodiscard]] const char* Data() const { return dataString.c_str();}
	private:
		std::string dataString;
	};
}
#endif
