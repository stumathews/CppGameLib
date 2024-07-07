#pragma once
#ifndef BITFIELDREADER_H
#define BITFIELDREADER_H
#include "utils/BitFiddler.h"

namespace gamelib
{
/**
	 * \brief Reads bits from a bitfield
	 * \tparam T length of the bitfield
	 */
	template <typename T>
	class BitfieldReader
	{
	public:
		explicit BitfieldReader(T* field, const size_t elements): field(field), elements(elements) { }		
				
		void Finish()
		{
			// This call should be used at the same sequence as calls to packer's Finish method to cater for its effects while reading from the buffer
			ReadNext<T>(bitsLeftInSegment);
		}

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
			const auto maxStartBit = fieldSizeBits-1;
			
			if(numBits >= (fieldSizeBits * 2)) throw std::exception("Can't read more than the double the buffer size.");

			if(countTimesOverflowed >= elements) throw std::exception("Buffer overflow.");

			// Trying to read outside the buffer, we'll need to read some from previous buffer and this buffer
			// to make a buffer that crosses both - a merged buffer unit
			if(startBit > maxStartBit)
			{							
				countTimesOverflowed++; segmentsRead++;
				return MakeMergedUnit<OType>(numBits, startBit);
			}

			// The range of bits falls within the buffer unit size (T)
			auto bitsValue = BitFiddler<T>::GetBitsValue(*(field+countTimesOverflowed), startBit, numBits);

			// How many bits were read off the buffer
			bitsRead += numBits;

			// Track the total number of bits we've read fo far
			totalBitsRead += numBits;

			// How many bits still left in the buffer unit segment
			bitsLeftInSegment = fieldSizeBits - bitsRead;

			// If we read up to the boundary, consider that we need to increase the overflow counter
			// this will enable us to read the next buffer unit.
			if(bitsRead % fieldSizeBits == 0)
			{
				countTimesOverflowed++; segmentsRead++;
				bitsLeftInSegment = fieldSizeBits;
				bitsRead = 0;
			}
			return static_cast<OType>(bitsValue);
		}

		void FetchBytes(char* outBuffer, const int outBufferSize)
		{
			if(bitsLeftInSegment != fieldSizeBits)
			{				
				countTimesOverflowed++; segmentsRead++;
				totalBitsRead += bitsLeftInSegment;
				bitsLeftInSegment = fieldSizeBits;
				bitsRead = 0;
			}

			memcpy_s(outBuffer, outBufferSize, field+countTimesOverflowed, outBufferSize);
			segmentsRead += (outBufferSize*8)/fieldSizeBits;
			bitsRead += (outBufferSize*8) % fieldSizeBits;
			totalBitsRead += (outBufferSize*8);
			countTimesOverflowed = segmentsRead;
			bitsLeftInSegment = fieldSizeBits - bitsRead;
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
			if(numBits >= (fieldSizeBits * 2)) throw std::exception("Can't read more than the double the buffer size.");

			if(startHighBit > fieldSizeBits)
			{
				// Can't read all of the value from within the current segment, 
				return MakeMergedUnit<OType>(numBits, startHighBit);
			}

			return static_cast<OType>(BitFiddler<T>::GetBitsValue(*(field), startHighBit, numBits));
		}

		/**
		 * \brief Restarts the reader's position at bit 0
		 */
		void Reset() { bitsRead = bitsLeftInSegment = countTimesOverflowed = 0; }

		uint8_t BitsLeftInSegment() const
		{
			return bitsLeftInSegment;
		}
		uint8_t SegmentsRead() const { return segmentsRead; }
		T TotalBitsRead() { return totalBitsRead;} 

	private:
		template <typename OType>
		OType MakeMergedUnit(const size_t numBits, unsigned long long startBit)
		{
			// Can't read all of the value from within the current segment:

			// LHS [1]  RHS [0]
			// 00000001 001_11000
			
			// 1111110000000100 0010010000101011
			//               ^       ^

			// we are going construct a new T from bits from the current block and some bits that overflowed and that exist in the next block
			std::remove_const_t<T> merged = (merged & 0); // note we remove const if T is const to make a non-const T that we can modify
			
			auto maxStartBit = (fieldSizeBits-1);
			// work out how many bits come from the left hand side (lhs) and how many from right hand side (rhs)				

			auto numBitsLhs = startBit - (fieldSizeBits-1);
			auto numBitsRhs = numBits - numBitsLhs;

			// get the bits from the rhs buffer and put them into the new T
				merged = BitFiddler<T>::GetBitsValue(*(field+countTimesOverflowed-1), maxStartBit, numBitsRhs);

			// put the bits from the next buffer after them in T, so T looks like this |lhsbits|rhsbits|
			merged = BitFiddler<T>::SetBits(merged, (numBitsRhs+numBitsLhs)-1, numBitsLhs, *(field+countTimesOverflowed));

			// If this is exactly a multiple of base type for the buffer, eg bit 61 down to 31 (and base of buffer is uint32_t)
			// no need to shift as there are 0 bits in the rhs and all thebits in the lhs
			const auto isMultipleOfT = (startBit-1) % (fieldSizeBits-1) == 0;

			if(!isMultipleOfT) 
			{
				// mask out/unset all other bits to 0
				merged = merged & (1 << numBits) -1;
			}

			// say that we must start reading from the next block (lhs) after the bits we've already used from it
			// this is so that the next read, does not re-read the bits we've already borrowed from it to form the new merged T block
			bitsRead = numBitsLhs;
			
			bitsLeftInSegment = fieldSizeBits - bitsRead;

			totalBitsRead += numBits;
							
			// return the merged block, T
			return merged;
		}
		uint8_t bitsRead {0};
		uint8_t segmentsRead {0};
		uint8_t bitsLeftInSegment {0};
		uint8_t totalBitsRead {0};
		T* field;
		uint8_t fieldSizeBits {sizeof(T)*8};
		uint8_t countTimesOverflowed {0};
		const size_t elements;
	};

}

#endif
