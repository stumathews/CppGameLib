#pragma once
#include <cstdint>
#include <utils/BitFiddler.h>

namespace gamelib
{
	/**
	 * \brief Tightly packs a unit of data with bits
	 * \tparam T type of unit of bit-packed data
	 */
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
		void Pack(uint8_t numBits, T value)
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
		uint8_t bufferSize;
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
		explicit BitfieldReader(T* field): field(field) { }

		/**
		 * \brief Reads the next set of bits since the last call
		 * \tparam OType return type of the value housing the bits read
		 * \param numBits number of bits more to read
		 * \return value with bits set
		 */
		template <typename OType>
		OType ReadNext(const size_t numBits)
		{
			auto startBit = (bitsRead + numBits) - 1;

			if(startBit > fieldSizeBits)
			{
				countTimesOverflowed++;

				// we are going construct a new T from bits from the current block and some bits that overflowed and that exist in the next block
				T merged = (merged & 0);

				auto rhsLastIndex = (fieldSizeBits-1);
				// work out how many bits come from the left hand side (lhs) and how many from right hand side (rhs)				

				auto numBitsLhs = startBit - (fieldSizeBits-1);
				auto numBitsRhs = numBits - numBitsLhs;

				// get the bits from the rhs buffer and put them into the new T
				merged = BitFiddler<T>::GetBitsValue(*field, rhsLastIndex, numBitsRhs);
				// put the bits from the next buffer after them in T, so T looks like this |lhsbits|rhsbits|
				merged = BitFiddler<T>::SetBits(merged, numBitsRhs+1, numBitsLhs, *field+countTimesOverflowed);

				// mask out/unset all other bits to 0
				merged = merged & (1 << numBits) -1;

				// say that we must start reading from the next block (lhs) after the bits we've already used from it
				// this is so that the next read, does not re-read the bits we've already borrowed from it to form the new merged T block
				bitsRead = numBitsLhs;

				// return the merged block, T
				return merged;
			}

			auto bitsValue = BitFiddler<T>::GetBitsValue(*(field+countTimesOverflowed), startBit, numBits);
			auto peek = BitFiddler<T>::ToString(bitsValue);
			bitsRead += numBits;

			return static_cast<OType>(bitsValue);
		}

		/**
		 * \brief Returns an interval of bits from starting at high-bit position backwards for numBits bits
		 * \tparam OType return type of the value housing the bits read
		 * \param startHighBit high bit
		 * \param numBits range of bits before and up to the high-bit
		 * \return value with bits read from the interval specified
		 */
		template<typename OType>
		OType ReadInterval(const size_t startHighBit, const size_t numBits)
		{
			return static_cast<OType>(BitFiddler<T>::GetBitsValue(*field, startHighBit, numBits));
		}

		/**
		 * \brief Restarts the reader's position at bit 0
		 */
		void Reset() { bitsRead = 0; }

	private:
		uint8_t bitsRead = 0;
		T* field;
		uint8_t fieldSizeBits {sizeof(T)*8};
		uint8_t countTimesOverflowed {0};
	};
};

	
	

