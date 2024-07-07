#pragma once
#ifndef BITPACKINGTYPES_H
#define BITPACKINGTYPES_H

#include <cstdint>
#include <ostream>
#include <sstream>

#include "BitfieldReader.h"
#include "BitPacker.h"

namespace gamelib
{
	namespace bit_packing_types
	{
		template <typename T>
		struct String
		{
			enum { MaxChars = 512 };

			private:

			// We store a fixed length 512 character string in memory
		    char elements[MaxChars] {0};
			int numElements{};
			int numBits {0};

			static void CheckLimits(const int count)
			{
				if(count > MaxChars) 
				{
					std::stringstream msg;
					msg << "Can only support " << MaxChars << " characters per string" << std::endl;
					throw std::exception(msg.str().c_str());
				}
			}

		public:		

			[[nodiscard]] char* c_str() const
			{
				return const_cast<char*>(elements);
			}

			explicit String(const std::string& value) { Initialize(value); }

			explicit String(const char* value) { Initialize(value); }

			String() : String(""){}

		    void Write(BitPacker<T>& bitPacker) const
		    {
				// Write how many elements are in this string
	    		bitPacker.Pack(sizeof(T)*8, static_cast<T>(numElements));

				// Push bytes into bitpacker's buffer directly
				bitPacker.PushBytes(elements, numElements);
		    }

		    void Read(BitfieldReader<T>& bitfieldReader)
		    {
				// read how many elemments in this string
		        numElements = bitfieldReader.ReadNext<int>(sizeof(T)*8);

				// Read byte directly from the reader's buffer
				bitfieldReader.FetchBytes(elements, numElements);

				elements[numElements] = '\0';
				numBits = NumBits();
		    }

			void Initialize(const std::string& string)
		    {
				CheckLimits(static_cast<int>(string.length()));

			    numElements = static_cast<int>(string.length());
				strcpy_s(elements,  string.c_str());
				numBits = NumBits();
		    }

			void Initialize(const char* string)
		    {
				CheckLimits(static_cast<int>(strlen(string)));

			    numElements = static_cast<int>(strlen(string));
				strcpy_s(elements,  string);
				numBits = NumBits();
		    }

			[[nodiscard]] int NumBits() const { return sizeof(T)*8 + (numElements * 8);}


		};

		template <typename T>
		struct ArrayOfStrings
		{		
			enum
			{
				MaxElements = 12
			};
			

			ArrayOfStrings(const std::string* buffer, const T numElements)
			{
				Initialize(buffer, numElements);
			}

			ArrayOfStrings() = default;

			void Initialize(const std::string* buffer, const T count)
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

			void Write(BitPacker<T>& bitPacker) const
			{
				// Store that there will be x elements in this array - we can't store/address more than MaxElements but can store less
				bitPacker.Pack(BITS_REQUIRED(0, MaxElements), numElements);

				// Now write each string
				for (int i = 0 ; i < numElements; i++)
				{
					elements[i].Write(bitPacker);
				}
			}

			void Read(BitfieldReader<T>& bitfieldReader)
		    {
				// Read how many elements there will be
				numElements = bitfieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0, MaxElements));
				for (int i = 0 ; i < numElements; i++)
				{
					elements[i].Read(bitfieldReader);
				}
			}

			String<T> operator[](const uint8_t index) const
			{
				return elements[index];
			}

		private:
			String<T> elements[MaxElements];
			uint16_t numElements{};
		};

	}

}

#endif
