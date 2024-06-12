#pragma once
#include <cstdint>
#include <bitset>

namespace gamelib
{
	#define GETMASK(index, size) ((((size_t)1 << (size)) - 1) << (index))
	#define READFROM(data, index, size) (((data) & GETMASK((index), (size))) >> (index))
	#define WRITETO(data, index, size, value) ((data) = (((data) & (~GETMASK((index), (size)))) | (((value) << (index)) & (GETMASK((index), (size))))))

	#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

	template <uint32_t x> struct PopCount
	{
	    enum { a = x - ( ( x >> 1 ) & 0x55555555 ),
	           b = ( ( ( a >> 2 ) & 0x33333333 ) + ( a & 0x33333333 ) ),
	           c = ( ( ( b >> 4 ) + b ) & 0x0f0f0f0f ),
	           d = c + ( c >> 8 ),
	           e = d + ( d >> 16 ),
	    result = e & 0x0000003f }; 
	};

	template <uint32_t x> struct Log2
	{
	    enum { a = x | ( x >> 1 ),
	           b = a | ( a >> 2 ),
	           c = b | ( b >> 4 ),
	           d = c | ( c >> 8 ),
	           e = d | ( d >> 16 ),
	           f = e >> 1,
	    result = PopCount<f>::result };
	};

	template <int64_t min, int64_t max> struct BitsRequired
	{
	    static const uint32_t result = 
	        ( min == max ) ? 0 : ( Log2<uint32_t(max-min)>::result + 1 );
	};



	template <class T>
	class BitFiddler
	{
	public:
		// Set bit n on number (zero-indexed)
		static T SetBit(T number, T n)
		{
			return number | ((T)1 << n);
		}

		// clear bt n on number
		static T ClearBit(T number, T n) {
			return number & ~((T)1 << n);
		}

		// toggle/invert bit n on number
		static T ToggleBit(T number, T n) {
	    return number ^ ((T)1 << n);
		}

		// test bit n if set on number
		static bool BitCheck(T number, T n)
		{
		    return (number >> n) & (T)1;
		}

		// set bit n or not
		static T SetBitTo(T number, T n, bool x) {
			return (number & ~((T)1 << n)) | ((T)x << n);
		}
		
		/**
		 * \brief 
		 * \param number 
		 * \param startBit high bit (zero indexed, 0 is the rightmost bit)
		 * \param numBits number bits towards lower bits to make interval
		 * \param zeroIndex 
		 * \return 
		 */
		static T GetBitsValue(const T number, const uint8_t startBit, const uint8_t numBits, const bool zeroIndex = true)
		{
			std::remove_const_t<T> number1 = number; // note we remove const if T is const to make a non-const T that we can modify

			if(startBit > 0 && startBit >= numBits)
				number1 = number1 >> (zeroIndex ? (((startBit+1)-numBits)) : (startBit - numBits)); // shift the bits we are interested to the beginning
			number1 = number1 & (1 << numBits) -1; // unset all but last numBits bits
			return number1;
		}

		/**
		 * \brief 
		 * \param number 
		 * \param startBit starting high-bit (zero indexed, 0 is the rightmost bit)
		 * \param bitLength length of interval towards lower bits
		 * \param newValue 
		 * \return 
		 */
		static T SetBits(T number, const uint8_t startBit, const uint8_t bitLength, const T newValue)
		{
			// get a mask that sets all bits in interval concerned
			std::remove_const_t<T> max = 0; // note we remove const if T is const to make a non-const T that we can modify
			max = ~(max & 0);

			// get a mask that sets all bits in interval concerned
			T mask = (max >> (sizeof(T)*8) - bitLength) 	<< startBit-(bitLength-1);

			// invert the mask so that that interval is all set to 0
			T i_mask = ~mask;

			std::remove_const_t<T> tempNumber = number; // note we remove const if T is const to make a non-const T that we can modify

			// apply mask to set the interval to all 0
			tempNumber &= i_mask;

			// apply the value in the interval
			tempNumber = tempNumber | newValue << startBit-(bitLength-1);

			return tempNumber;
		}


		static std::string ToString(T number, bool bigEndian = false)
		{
			const auto maxBits = sizeof(T) * 8;
			std::stringstream bitString;
			if(bigEndian)
			{
				for(int i = 0; i < maxBits; i++)
				{
					bitString << BitCheck(number, i) ? "1": "0";	
				}

				return bitString.str();
			}
			// little endian
			for(int i = maxBits-1; i >=0; i--)
			{
				bitString << BitCheck(number, i) ? "1": "0";	
			}

			return bitString.str();
		}
	};
}
