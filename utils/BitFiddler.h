#pragma once
#include <cstdint>

namespace gamelib
{
	template <class T>
	class BitFiddler
	{
	public:
		// Set bit n on number
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

		static std::string ToString(T number)
		{
			const auto maxBits = sizeof(T) * 8;
			std::stringstream bitString;
			for(int i = 0; i < maxBits -1; i++)
			{
				bitString << BitCheck(number, i) ? "1": "0";	
			}

			return bitString.str();
		}
	};
}
