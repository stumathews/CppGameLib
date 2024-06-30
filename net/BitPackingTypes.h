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

			[[nodiscard]] char* c_str() const { return const_cast<char*>(elements); }

			// ReSharper disable once CppNonExplicitConvertingConstructor
			String(const std::string& value) { Initialize(value); }

			// ReSharper disable once CppNonExplicitConvertingConstructor
			String(const char* value) { Initialize(value); }

			String() : String(""){}

		    void Write(gamelib::BitPacker<uint16_t>& bitPacker) const
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
				CheckLimits(static_cast<int>(string.length()));

			    numElements = static_cast<int>(string.length());
				strcpy(elements,  string.c_str());
				numBits = NumBits();
		    }

			void Initialize(const char* string)
		    {
				CheckLimits(static_cast<int>(strlen(string)));

			    numElements = static_cast<int>(strlen(string));
				strcpy(elements,  string);
				numBits = NumBits();
		    }

			[[nodiscard]] int NumBits() const { return 16 + (numElements * 8);}


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

			void Write(BitPacker<uint16_t>& bitPacker) const
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

}

#endif
