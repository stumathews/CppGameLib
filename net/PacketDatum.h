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
		explicit PacketDatum(bool acked, const char* customData = "", unsigned long sendTimeMs = 0);

		template <typename T>		
		void Write(BitPacker<T>& bitPacker) const
		{
			bitPacker.Pack(BITS_REQUIRED( 0, 1), Acked);
			bitPacker.Pack(16, Sequence);
			bit_packing_types::String<T> temp(dataString);
			temp.Write(bitPacker);
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

		[[nodiscard]] int EstimateSizeInBytes() const
		{
			// 1 + 16 + StringSize
			return 1 + 16 + bit_packing_types::String<uint16_t>::GetSizeEstimateInBytes(dataString.length());
		}

		bool Acked;
		uint16_t Sequence {};
		unsigned long SendTime {0}; // We can evaluate the RTT for this packet when it gets acked. This is not serialized
		[[nodiscard]] const char* Data() const { return dataString.c_str();}
	private:
		std::string dataString;
	};
}
#endif
