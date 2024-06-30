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

		BitPacker(T* flushDestination, const int element)
			: writeBitPointer(0), readBitPointer(0), flushDestination(flushDestination), destElement(element)
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

			buffer = BitFiddler<T>::SetBits(buffer, writeBitPointer+(numBits-1), numBits, value);
			auto peek = BitFiddler<T>::ToString(buffer);
			writeBitPointer += numBits;

			bitsPacked += numBits;

			if(writeBitPointer >= bufferSizeBits)
			{
				// Refuse to write passed destination
				if(countTimesOverflowed == destElement) throw std::exception("Can't write past destination buffer");

				// flush our buffer to the output buffer
				memcpy_s(flushDestination+(countTimesOverflowed++), bufferSizeBits/8, &buffer, bufferSizeBits/8);
								
				buffer = value;											

				// Determine which bits overflowed - shift off the bits that we were able to read, leaving those left unread in the buffer for the next packing
				// to append to
				buffer = buffer >> numBits - (writeBitPointer - bufferSizeBits);

				// set that we've read the previously overflowed bits in our buffer now
				writeBitPointer = (writeBitPointer - bufferSizeBits);
			}
		}

		// Writes the last buffered 16bits to destination and clears the buffer
		void Flush()
		{
			memcpy_s(flushDestination+(countTimesOverflowed++), bufferSizeBits/8, &buffer, bufferSizeBits/8);
			buffer = (buffer & 0);
		}

		void Finish()
		{
			Flush();
		}
		
		void Reset() { readBitPointer = writeBitPointer = countTimesOverflowed = 0; }
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
		int destElement;		
		uint8_t bufferSizeBits;
		uint8_t countTimesOverflowed {0};
		T bitsPacked {0};		
	};

};

	
	

