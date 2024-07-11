#pragma once
#include <cstdint>
#include <utils/BitFiddler.h>
#include <iostream>
#include <sstream>
#include <string>

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

		BitPacker(T* flushDestination, const int elements)
			: writeBitPointer(0), readBitPointer(0), flushDestination(flushDestination), destElements(elements)
		{
			buffer = (buffer & 0);
			bufferSizeBits = sizeof(T)*8;
		}

		// Pack the bits into the buffer. Buffer will auto flush when its full
		void Pack(uint8_t numBits, T value)
		{
			if(numBits > bufferSizeBits) 
			{
				std::stringstream message;
				message << "can't write " << std::to_string(numBits) << " bits to a " << std::to_string(bufferSizeBits) << " bit bitfield" << std::endl;
				throw std::exception(message.str().c_str());
			}

			// Where to start writing in out temp buffer?
			auto startBit = writeBitPointer+(numBits-1);
			auto maxStartBit = bufferSizeBits-1;

			// How much have we got left of internal buffer uint (T) to pack into?
			bitsLeftInSegment = bufferSizeBits - writeBitPointer;

			if(startBit > maxStartBit)
			{
				// can't fit into temp Buffer, we only have bitsLeftInSegment bits available

				// set the bitsLeftInSegment to what we can from the value. 
				buffer = BitFiddler<T>::SetBits(buffer, maxStartBit, bitsLeftInSegment, value);

				//The segment is now full, flush it 
				memcpy_s(flushDestination+(countTimesOverflowed++), bufferSizeBits/8, &buffer, bufferSizeBits/8);

				bitsPacked += bitsLeftInSegment;
				segmentsWritten++;

				// Take the remaining bits of the value we could not set (as the did not fix), and write them in a new buffer instead:
				buffer = (buffer & 0);
				
				auto extraBits = numBits - (bufferSizeBits - writeBitPointer);
				auto newStartBit = (numBits - (bufferSizeBits - writeBitPointer))-1;
				
				auto remainingBits  = BitFiddler<T>::GetBitsValue(value, numBits-1, extraBits);
				buffer = BitFiddler<T>::SetBits(buffer, newStartBit, extraBits, remainingBits);

				// we've set some bits  in the new current segment, write it to memory so the reader can read it.
				// otherwise the reader wont know about it until its auto flushed by completing the segment through more writes
				// which may not happen if this is the last write/pack
				memcpy_s(flushDestination+(countTimesOverflowed), bufferSizeBits/8, &buffer, bufferSizeBits/8);

				bitsPacked += extraBits;
				writeBitPointer = newStartBit+1;				
				bitsLeftInSegment = bufferSizeBits - extraBits;			

			}
			else
			{
				// nice, we can fit into buffer
				buffer = BitFiddler<T>::SetBits(buffer, startBit, numBits, value);
				
				// indicate that we packed that many bits into the buffer
				bitsPacked += numBits;
								
				if(bitsPacked % bufferSizeBits == 0)
				{
					// we packed up to a bit boundary, we'll need to increase overflow so we can access the next unit segment in the buffer.
					// We will do this and write the buffer at the same time
					memcpy_s(flushDestination+(countTimesOverflowed++), bufferSizeBits/8, &buffer, bufferSizeBits/8);
					segmentsWritten++;
					bitsLeftInSegment = bufferSizeBits;
					writeBitPointer = 0;
				}
				else
				{
					// move writeBitPointer up 1 past where we started
					writeBitPointer += numBits;
					bitsLeftInSegment = bufferSizeBits - writeBitPointer;
				}
			}
		}

		void PushBytes(const char* pushBuffer, const int pushBufferSize)
		{
			// skip bits until next segment, which should be a byte boundary
			if(bitsLeftInSegment != bufferSizeBits)
			{
				Flush();
			}

			// copy the string directly into the buffer - no bit packing
			memcpy_s(flushDestination+countTimesOverflowed, (destElements * bufferSizeBits)/8, pushBuffer, pushBufferSize);
						
			writeBitPointer = (pushBufferSize*8) % bufferSizeBits;			
			bitsPacked += (pushBufferSize*8);
			bitsLeftInSegment = bufferSizeBits - writeBitPointer;
			segmentsWritten += (pushBufferSize*8)/bufferSizeBits;
			countTimesOverflowed = segmentsWritten;

			// Set the current buffer to the last segment we wrote with memcpy
			// because on subsequent packs it assumes the buffer contains the last segment written
			// which has changed as we've written many segments with memcpy
			memcpy_s(&buffer, bufferSizeBits/8, flushDestination+countTimesOverflowed, bufferSizeBits/8);

		}

		// Writes the last buffered 16bits to destination and clears the buffer
		void Flush(bool readUnusedBits = true)
		{
			memcpy_s(flushDestination+(countTimesOverflowed++), bufferSizeBits/8, &buffer, bufferSizeBits/8);
			buffer = (buffer & 0);
			
			// consider the remaining bits in the segment as being written as we flushed the whole segment to memory
			bitsPacked += bitsLeftInSegment;
			
			writeBitPointer = 0;			

			// having flushed the whole segment, we should have all the bits unread in the new segment
			bitsLeftInSegment = bufferSizeBits - writeBitPointer;

			segmentsWritten = countTimesOverflowed;
		}

		void Finish()
		{
			Flush();
		}

		T TotalSegmentsWritten() { return segmentsWritten; }
		T BitsLeftInCurrentSegment() { return bitsLeftInSegment; }
		
		void Reset() { readBitPointer = writeBitPointer = countTimesOverflowed = bitsPacked = segmentsWritten = 0; }
		T TotalBitsPacked() { return bitsPacked; }
		bool HasOverflowed {false};

		static constexpr unsigned BitsToRepresent(uintmax_t n) 
		{
		    return n > 0 
		        ? 1 + BitsToRepresent(n/2)
		        : 0; 
		}

		static constexpr unsigned BitsRequired(intmax_t min,intmax_t max)
		{
		    return BitsToRepresent(static_cast<uintmax_t>(max) - static_cast<uintmax_t>(min));
		}



	private:
		T buffer;
		uint8_t writeBitPointer;
		uint8_t readBitPointer;		
		T* flushDestination;
		int destElements;		
		uint8_t bufferSizeBits;
		uint8_t countTimesOverflowed {0};
		T bitsPacked {0};
		uint8_t segmentsWritten = 0;
		uint8_t bitsLeftInSegment = 0;


	};

};

	
	

