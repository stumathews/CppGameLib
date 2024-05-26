#pragma once
#include <cstdint>
#include <utils/BitFiddler.h>

namespace gamelib
{
	
	/**
	 * Write the the bits read off the network into this structure
	 */
	template <class T>
	class BitWriter
	{
		public:
		BitWriter(const int packetSize, T* pPacket) : scratch_bits(0), word_index(0), m_packetSize(packetSize),
		                                                 m_pPacket(pPacket)
		{
			//total_bits = packetSize * 8;
		}

		// read numBits from the packet
		void Write(const int numBits, T* outputPacketDestination)
		{
			scratch[0] = scratch[1] = {0};
			scratch_bits = {0};
			word_index = {0};

			// pBuffer points to the start of the packet we are writing to
			pBuffer = static_cast<T*>(outputPacketDestination);

			// write numBits from m_pPacket into scratch

			auto gotBitValue = BitFiddler<T>::GetBitsValue(*m_pPacket, scratch_bits+1, numBits);

			BitFiddler<T>::SetBits(scratch[0], scratch_bits+1, numBits, gotBitValue);

			int i = 0;

			scratch_bits += numBits;

			memcpy_s(outputPacketDestination, sizeof(T), &scratch, sizeof(T));

		}
		private:
		T scratch[2];
		int scratch_bits;
		int word_index;
		T * pBuffer {nullptr};	

		const int m_packetSize {0};
		T* m_pPacket {0};
	};

	/**
	 * Reads the bits in this structure in order to write those bits to an external structure like a packet
	 */
	template <class T>
	class BitReader
	{
	public:
		

	void WriteBits(BitWriter<T>& writer, int numElements, int numBits = 0)
	{
		
	}

	void ReadBits(BitReader<T>& reader, int numElements, int numBits = 0)
	{
		
	}

		
	private:
		uint64_t scratch {0};
		int scratch_bits {0};
		int total_bits {0};
		int num_bits_read {0};
		int word_index {0};
		uint32_t * pbuffer {nullptr};
	};

};

	
	

