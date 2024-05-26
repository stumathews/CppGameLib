#pragma once
#include <cstdint>
#include <utils/BitFiddler.h>

namespace gamelib
{

	template <class T>
	class BitPacker
	{
	public:

		BitPacker(T* flushDestination, int element)
			: readBitPointer(0), writeBitPointer(0), flushDestination(flushDestination), destElement(element)
		{
			buffer = (buffer & 0);
			bufferSize = sizeof(T)*8;
		}

		void Pack(size_t numBits, T value)
		{			
			buffer = BitFiddler<T>::SetBits(buffer, readBitPointer+(numBits-1), numBits, value);
			readBitPointer += numBits;
						
			if(readBitPointer >= bufferSize)
			{
				auto peek = BitFiddler<T>::ToString(buffer);
				// Write out the buffer to output
				memcpy_s(flushDestination+(countTimesOverflowed++), bufferSize/8, &buffer, bufferSize/8);
				
				buffer = value;				
				peek = BitFiddler<T>::ToString(buffer);
								
				
				buffer = buffer >> numBits - (readBitPointer - bufferSize);
				peek = BitFiddler<T>::ToString(buffer);
				int i = 0;
				readBitPointer = (readBitPointer - bufferSize);
			}
		}

		T UnPack(const size_t numBits)
		{
			T temp = BitFiddler<T>::GetBitsValue(buffer, writeBitPointer + (numBits-1), numBits);
			writeBitPointer += numBits;
			return temp;
		}

		void Reset() { writeBitPointer = readBitPointer = countTimesOverflowed = 0; }
		[[nodiscard]] int GetUsedBits() const { return readBitPointer; }
		
		bool HasOverflowed {false};

	private:
		T buffer;
		uint8_t readBitPointer;
		uint8_t writeBitPointer;		
		T* flushDestination;
		int destElement;		
		size_t bufferSize;
		uint8_t countTimesOverflowed {0};
	};

	/**
	 * Write the the bits read off the network into this structure
	 */
	template <class T>
	class BitWriter
	{
		public:
		BitWriter(const int packetSize, T* networkBuffer) : scratch_bits(0), word_index(0), m_packetSize(packetSize),
		                                                 m_pNetworkBuffer(networkBuffer)
		{

		}

		// read numBits from the packet
		void Write(const int numBits, void* destination)
		{
			scratch[0] = scratch[1] = {0};
			scratch_bits = {0};
			word_index = {0};

			// pBuffer points to the start of the packet we are writing to
			pBuffer = static_cast<T*>(destination);

			// write numBits from m_pPacket into scratch
			T bitValue = BitFiddler<T>::GetBitsValue(*m_pNetworkBuffer, scratch_bits, numBits);

			scratch[0] = BitFiddler<T>::SetBits(scratch[0], scratch_bits, numBits, 
				bitValue);

			std::cout << scratch[0];
			
			scratch_bits += numBits;
		}
		private:
		T scratch[2];
		int scratch_bits;
		int word_index;
		T * pBuffer {nullptr};	

		const int m_packetSize {0};
		T* m_pNetworkBuffer {0};
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

	
	

