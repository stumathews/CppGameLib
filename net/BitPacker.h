#pragma once
#include <cstdint>
#include <utils/BitFiddler.h>

namespace gamelib
{

	template <class T>
	class BitPacker
	{
	public:

		BitPacker(T* flushDestination, const int element)
			: writeBitPointer(0), readBitPointer(0), flushDestination(flushDestination), destElement(element)
		{
			buffer = (buffer & 0);
			bufferSize = sizeof(T)*8;
		}

		// Pack the bits into the buffer. Buffer will auto flush when its full
		void Pack(size_t numBits, T value)
		{			
			buffer = BitFiddler<T>::SetBits(buffer, writeBitPointer+(numBits-1), numBits, value);
			writeBitPointer += numBits;

			bitsPacked += numBits;

			if(writeBitPointer >= bufferSize)
			{
				// flush our buffer to the output buffer
				memcpy_s(flushDestination+(countTimesOverflowed++), bufferSize/8, &buffer, bufferSize/8);
								
				buffer = value;											

				// Determine which bits overflowed - shift off the bits that we were able to read, leaving those left unread in the buffer for the next packing
				// to append to
				buffer = buffer >> numBits - (writeBitPointer - bufferSize);

				// set that we've read the previously overflowed bits in our buffer now
				writeBitPointer = (writeBitPointer - bufferSize);
			}
		}

		// Writes the last buffered 16bits to destination and clears the buffer
		void Flush()
		{
			memcpy_s(flushDestination+(countTimesOverflowed++), bufferSize/8, &buffer, bufferSize/8);
			buffer = (buffer & 0);
		}
		
		void Reset() { readBitPointer = writeBitPointer = countTimesOverflowed = 0; }
		T TotalBitsPacked() { return bitsPacked; }
		bool HasOverflowed {false};

	private:
		T buffer;
		uint8_t writeBitPointer;
		uint8_t readBitPointer;		
		T* flushDestination;
		int destElement;		
		size_t bufferSize;
		uint8_t countTimesOverflowed {0};
		T bitsPacked {0};
	};

	/**
	 * \brief Reads bits from a bitfield
	 * \tparam T length of the bitfield
	 */
	template <typename T>
	class BitfieldReader
	{
	public:
		explicit BitfieldReader(T* field): field(field), fieldSizeBits(sizeof(T)*8) { }

		template <typename OType>
		OType ReadNext(const size_t numBits)
		{
			const auto startBit = (bitsRead + numBits) - 1;

			if(startBit > fieldSizeBits) throw std::exception("Tried to read past bitfield boundary");

			auto bitsValue = BitFiddler<uint32_t>::GetBitsValue(*field, startBit, numBits);
			bitsRead += numBits;

			return (OType)bitsValue;
		}

		// Returns an interval of bits from starting at high-bit position backwards for numBits bits
		template<typename OType>
		OType ReadInterval(const size_t startHighBit, const size_t numBits)
		{
			return (OType) BitFiddler<uint32_t>::GetBitsValue(*field, startHighBit, numBits);
		}

		// Restarts the reader's position at bit 0
		void Reset() { bitsRead = 0; }

	private:
		size_t bitsRead = 0;
		T* field;
		size_t fieldSizeBits {0};
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

	
	

