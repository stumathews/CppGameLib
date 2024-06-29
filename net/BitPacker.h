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

	/**
	 * \brief Reads bits from a bitfield
	 * \tparam T length of the bitfield
	 */
	template <typename T>
	class BitfieldReader
	{
	public:
		explicit BitfieldReader(T* field, const size_t elements): field(field), elements(elements) { }

		

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

			if(numBits >= (fieldSizeBits * 2)) throw std::exception("Can't read more than the double the buffer size.");

			if(countTimesOverflowed >= elements) throw std::exception("Buffer overflow.");
			
			if(startBit > fieldSizeBits-1)
			{							
				countTimesOverflowed++;

				return MakeMergedUnit<OType>(numBits, startBit);
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
			if(numBits >= (fieldSizeBits * 2)) throw std::exception("Can't read more than the double the buffer size.");

			if(startHighBit > fieldSizeBits)
			{						
				return MakeMergedUnit<OType>(numBits, startHighBit);
			}

			return static_cast<OType>(BitFiddler<T>::GetBitsValue(*(field), startHighBit, numBits));
		}

		/**
		 * \brief Restarts the reader's position at bit 0
		 */
		void Reset() { bitsRead = 0; }

	private:
		template <typename OType>
		OType MakeMergedUnit(const size_t numBits, unsigned long long startBit)
		{
			// LHS [1]  RHS [0]
			// 00000001 001_11000
			
			// 1111110000000100 0010010000101011
			//               ^       ^

			// we are going construct a new T from bits from the current block and some bits that overflowed and that exist in the next block
			std::remove_const_t<T> merged = (merged & 0); // note we remove const if T is const to make a non-const T that we can modify

			auto rhsLastIndex = (fieldSizeBits-1);
			// work out how many bits come from the left hand side (lhs) and how many from right hand side (rhs)				

			auto numBitsLhs = startBit - (fieldSizeBits-1);
			auto numBitsRhs = numBits - numBitsLhs;

				// get the bits from the rhs buffer and put them into the new T
				merged = BitFiddler<T>::GetBitsValue(*(field+countTimesOverflowed-1), rhsLastIndex, numBitsRhs);
			
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
				
			// return the merged block, T
			return merged;
		}
		uint8_t bitsRead {0};
		uint8_t totalBitsRead {0};
		T* field;
		uint8_t fieldSizeBits {sizeof(T)*8};
		uint8_t countTimesOverflowed {0};
		const size_t elements;
	};

	namespace bit_packing_types
	{
		struct String
	{
		enum
		{
			MaxChars = 512
		};


		[[nodiscard]] char* c_str() const { return const_cast<char*>(elements); }

		String(std::string value) { Initialize(value); }
		String(const char* value) { Initialize(value); }
		String() : String(""){}

	    void Write(BitPacker<uint16_t>& bitPacker)
	    {
	    	bitPacker.Pack(16, static_cast<uint16_t>(numElements));
			
			for(int i = 0; i < numElements;i++)
			{
				bitPacker.Pack(8, elements[i]);
			}
	    }

	    void Read(BitfieldReader<uint16_t>& bitfieldReader)
	    {
	        numElements = bitfieldReader.ReadNext<int>(16);
			for(int i = 0; i < numElements;i++)
			{
				elements[i] = bitfieldReader.ReadNext<char>(8);
			}
			numBits = NumBits();
	    }

		void Initialize(const std::string& string)
	    {
			CheckLimits(string.length());

		    numElements = static_cast<int>(string.length());
			strcpy(elements,  string.c_str());
			numBits = NumBits();
	    }

		void Initialize(const char* string)
	    {
			CheckLimits(strlen(string));

		    numElements = static_cast<int>(strlen(string));
			strcpy(elements,  string);
			numBits = NumBits();
	    }

		[[nodiscard]] int NumBits() const { return 16 + (numElements * 8);}

		private:

		static void CheckLimits(const int count)
		{
			if(count > MaxChars) 
			{
				std::stringstream msg;
				msg << "Can only support " << MaxChars << " characters per string" << std::endl;
				throw std::exception(msg.str().c_str());
			}
		}
		int numElements{};
	    char elements[MaxChars] {0};
		int numBits {0};

	};

	struct ArrayOfStrings
	{		
		enum
		{
			MaxElements = 12
		};
		

		ArrayOfStrings(const std::string* buffer, const uint16_t numElements)
		{
			Initialize(buffer, numElements);
		}

		ArrayOfStrings() = default;

		void Initialize(const std::string* buffer, const uint16_t count)
		{
			if(count > MaxElements) 
			{
				std::stringstream msg;
				msg << "Can only support " << MaxElements << " strings in this array" << std::endl;
				throw std::exception(msg.str().c_str());
			}

			numElements = count;
			for(int i = 0; i < count;i++)
			{
				elements[i].Initialize(buffer[i]);
			}			
		}

		void Write(BitPacker<uint16_t>& bitPacker)
		{
			bitPacker.Pack(BITS_REQUIRED(0, MaxElements), numElements);
			for (int i = 0 ; i < numElements; i++)
			{
				elements[i].Write(bitPacker);
			}
		}

		void Read(BitfieldReader<uint16_t>& bitfieldReader)
	    {
			numElements = bitfieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0, MaxElements));
			for (int i = 0 ; i < numElements; i++)
			{
				elements[i].Read(bitfieldReader);
			}
		}

		bit_packing_types::String operator[](const uint8_t index) const
		{
			return elements[index];
		}

	private:
		bit_packing_types::String elements[MaxElements];
		uint16_t numElements{};
	};

	}

};

	
	

