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
			private:
				
		    std::vector<char> elements;
			int numElements{};
			int numBits {0};

		public:		

			[[nodiscard]] char* c_str() const
			{
				return const_cast<char*>(&elements[0]);
			}

			explicit String(const std::string& value) { Initialize(value); }

			explicit String(const char* value) { Initialize(value); }

			String() : String(""){}

		    void Write(BitPacker<T>& bitPacker) const
		    {
				// Write how many elements are in this string
	    		bitPacker.Pack(sizeof(T)*8, static_cast<T>(numElements));

				// Push bytes into bitpacker's buffer directly
				bitPacker.PushBytes(&elements[0], numElements);
		    }

		    void Read(BitfieldReader<T>& bitfieldReader)
		    {
				// read how many elemments in this string
		        numElements = bitfieldReader.ReadNext<int>(sizeof(T)*8);

				elements.resize(numElements);

				// Read byte directly from the reader's buffer
				bitfieldReader.FetchBytes(&elements[0], numElements);

				elements.push_back('\0');
				numBits = NumBits();
		    }

			void Initialize(const std::string& string)
		    {
				
				numElements = static_cast<int>(string.length());
				elements.resize(numElements);

				elements.assign(string.begin(), string.end());
				elements.push_back('\0');
				numBits = NumBits();
		    }

			void Initialize(const char* string)
		    {
				numElements = static_cast<int>(strlen(string));
				elements.insert(elements.end(), string, string+numElements);
				elements.push_back('\0');
				numBits = NumBits();
		    }

			[[nodiscard]] int NumBits() const { return sizeof(T)*8 + (numElements * 8);}
		};

		template <typename T>
		struct ByteArray
		{
			
			private:
				
		    std::vector<char> elements;
			int numElements{};
			int numBits {0};

		public:		

			[[nodiscard]] char* data() const { return const_cast<char*>(&elements[0]);}
			[[nodiscard]] size_t Size() const { return numElements;}
			
			explicit ByteArray(const char* value, int length) { Initialize(value, length); }

			ByteArray() : ByteArray("", 0)
			{
				
			}

		    void Write(BitPacker<T>& bitPacker) const
		    {
				// Write how many elements are in this string
	    		bitPacker.Pack(sizeof(T)*8, static_cast<T>(numElements));

				// Push bytes into bitpacker's buffer directly
				bitPacker.PushBytes(&elements[0], numElements);
		    }

		    void Read(BitfieldReader<T>& bitfieldReader)
		    {
				// read how many elemments in this string
		        numElements = bitfieldReader.ReadNext<int>(sizeof(T)*8);

				elements.resize(numElements);

				// Read byte directly from the reader's buffer
				bitfieldReader.FetchBytes(&elements[0], numElements);
				
				numBits = NumBits();
		    }

			void Initialize(const char* string, const int size)
		    {
				numElements = size;
				elements.insert(elements.end(), string, string+numElements);
				numBits = NumBits();
		    }

			[[nodiscard]] int NumBits() const { return sizeof(T)*8 + (numElements * 8);}
		};

		template <typename T>
		struct ArrayOfStrings
		{		
			enum
			{
				// We support storing up to 12 variable length strings. Should be enough
				MaxElements = 12
			};
			
			ArrayOfStrings(const std::string* buffer, const T numElements)
			{
				Initialize(buffer, numElements);
			}

			ArrayOfStrings() = default;

			void Initialize(const std::string* buffer, const T count)
			{
				numElements = count;
				for(int i = 0; i < count;i++)
				{
					elements.push_back(String<T>(buffer[i]));
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
					String<T> string;
					string.Read(bitfieldReader);
					elements.push_back(string);
				}
			}

			String<T> operator[](const uint8_t index) const
			{
				return elements[index];
			}

			uint8_t NumElements() const { return elements.size(); }

		private:
			std::vector<String<T>> elements;
			uint16_t numElements{};
		};

	}

}

#endif
