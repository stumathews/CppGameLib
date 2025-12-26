#include "pch.h"
#include "utils/BitFiddler.h"
#include <limits>
#include <gtest/gtest.h>

namespace gamelib
{

	class BitFiddlerTests : public testing::Test
	{
	public:	

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};


	TEST_F(BitFiddlerTests, ToString)
	{
		uint8_t byte = 0; // Max 0 to 255

		byte = 255; //11111111

		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "11111111");

		byte = 5;

		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "00000101");

		byte = 9;

		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "00001001");
		

		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte << 4), "10010000"); //144 in binary
		EXPECT_EQ(byte << 4, 144);
		
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte << 1), "00010010"); //10 in binary
		EXPECT_EQ(byte << 1, 18);

		constexpr uint16_t packetBuffer = 62139; // 1111001010111011
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(packetBuffer), "1111001010111011"); //62139 in binary
	}

	TEST_F(BitFiddlerTests, GetBitsValue)
	{		
		// git bits from a 8bit field
		uint8_t byte = 0; // Max 0 to 255

		byte = 187;
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "10111011"); //144 in binary

		// LEFT: get 1 bits from bit 0 (zero-indexed)	where bit 0 is the right most bit;
		// RIGHT: get 1 bits from bit 1 (1-based) where bit 1 is the right most bit
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 0, 1), 1); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 1, 1, false), 1);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 1, 1), 1); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 2, 1, false), 1);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 2, 1), 0); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 3, 1, false), 0);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 2, 2), 1); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 3, 2, false), 1);

		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 2, 3), 3); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 3, 3, false), 3);

		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 5, 1), 1); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 6, 1, false), 1);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 5, 2), 3); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 6, 2, false), 3);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 5, 3), 7); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 6, 3, false), 7);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 5, 0), 0); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 6, 0, false), 0);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 3, 4), 11); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 4, 4, false), 11);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 4, 5), 27); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 5, 5, false), 27);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 7, 3), 5); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 8, 3, false), 5);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 3, 3), 5); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 4, 3, false), 5);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 6, 4), 7); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 7, 4, false), 7);
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 7, 8), 187); EXPECT_EQ(gamelib::BitFiddler<uint8_t>::GetBitsValue(byte, 8, 8, false), 187);

		// get bits from a 16-bit field

		uint16_t packetBuffer = 62139; // 11110010 10111011
		EXPECT_EQ(packetBuffer, 62139);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(packetBuffer), "1111001010111011"); //62139 in binary

		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(gamelib::BitFiddler<uint16_t>::GetBitsValue(packetBuffer, 1, 1)), "0000000000000001");
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(gamelib::BitFiddler<uint16_t>::GetBitsValue(packetBuffer, 5, 3)), "0000000000000111");
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::GetBitsValue(packetBuffer, 1, 2), 3);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::GetBitsValue(packetBuffer, 2, 3), 3);

	}

	TEST_F(BitFiddlerTests, SetBits)
	{
		uint8_t byte = 0; // Max 0 to 255

		byte = 187;
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "10111011"); //144 in binary
		

		#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result
			
		// write 5 (101) which is 3 bits long at bits 3 of byte (10111011) so it becomes 10101011
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(gamelib::BitFiddler<uint8_t>::SetBits(byte, 5, 3, 5 )), "10101011");
			
		// Note: we can use BITS_REQUIRED to solve figure out that 101 needs 3 bits) 
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::SetBits(byte, 5, BITS_REQUIRED(0, 5), 5 ), 171);

		byte = 0; //00011000
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(gamelib::BitFiddler<uint8_t>::SetBits(byte, 5, 3, 6 )), "00110000");

		uint16_t packetBuffer = 62139; // 1111001010111011
		//                                          ^              ^
		//                                       bit 16          bit 0
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(gamelib::BitFiddler<uint16_t>::SetBits(packetBuffer, 15, 1, 0 )), "0111001010111011");
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(gamelib::BitFiddler<uint16_t>::SetBits(packetBuffer, 4, 3, 7 )), "1111001010111111");
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(gamelib::BitFiddler<uint16_t>::SetBits(packetBuffer, 16, 8, 255 )), "1111111010111011");

		
		packetBuffer = (packetBuffer & 0); // 0000000000000000
		
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(BitFiddler<uint16_t>::SetBits(packetBuffer, 1, 2, 2)), "0000000000000010");
	}

	TEST_F(BitFiddlerTests, SetBit)
	{
		constexpr int i = 1;
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(i), "00000000000000000000000000000001");
		
		// setting bit 20 (bit 0 is the last digit on the right, i.e we're little-endian)
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(i << (20-1)), "00000000000010000000000000000000");
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(gamelib::BitFiddler<int>::SetBit(i,20-1)), "00000000000010000000000000000001");
		//setting bit 2
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(i << (2-1)), "00000000000000000000000000000010");
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(gamelib::BitFiddler<int>::SetBit(i,2-1)), "00000000000000000000000000000011");
		//setting bit 3
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(i << (3-1)), "00000000000000000000000000000100");
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(gamelib::BitFiddler<int>::SetBit(i,3-1)), "00000000000000000000000000000101");
		// ...
		//setting bit 9
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(i << (9-1)), "00000000000000000000000100000000");
		EXPECT_EQ(gamelib::BitFiddler<int>::ToString(gamelib::BitFiddler<int>::SetBit(i,9-1)), "00000000000000000000000100000001");

		constexpr uint8_t byte = 0;
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "00000000");
		// set bit 1
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(i << (1-1)), "00000001");
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(gamelib::BitFiddler<uint8_t>::SetBit(i,1-1)), "00000001");
		// set bit 2
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(i << (2-1)), "00000010");
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(gamelib::BitFiddler<uint8_t>::SetBit(i,2-1)), "00000011");
		// set bit 3
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(i << (3-1)), "00000100");
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(gamelib::BitFiddler<uint8_t>::SetBit(i,3-1)), "00000101");


	}

	TEST_F(BitFiddlerTests, ReadMask)
	{
		uint32_t i = 1;
		EXPECT_EQ(gamelib::BitFiddler<uint32_t>::ToString(i), "00000000000000000000000000000001");
		i = BitFiddler<uint32_t>::SetBit(i,12-1);
		i = BitFiddler<uint32_t>::SetBit(i,13-1);
		i = BitFiddler<uint32_t>::SetBit(i,14-1);
		EXPECT_EQ(gamelib::BitFiddler<uint32_t>::ToString(i), "00000000000000000011100000000001");
		//  134234111 =                                        00001000000000000011111111111111
		i = i | 134234111; // set: all bits that are 1 in 134234111 to 1 in i
		EXPECT_EQ(gamelib::BitFiddler<uint32_t>::ToString(i), "00001000000000000011111111111111");
		// 134220035 =                                         00001000000000000000100100000011
		i = i & 134220035; // unset: all bits that are 0 in 3 to 0 in i
		EXPECT_EQ(gamelib::BitFiddler<uint32_t>::ToString(i), "00001000000000000000100100000011");
		// get bit value at 12-9
		i = i >> (9-1);
		EXPECT_EQ(gamelib::BitFiddler<uint32_t>::ToString(i), "00000000000010000000000000001001");
		// 15 =                                                00000000000000000000000000001111
		i = i & (1 << 4) - 1; // get last 4 bits
		EXPECT_EQ(gamelib::BitFiddler<uint32_t>::ToString(i), "00000000000000000000000000001001");

		uint16_t t = 62139; // 11110010 10111011
		EXPECT_EQ(t, 62139);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(t), "1111001010111011"); //62139 in binary

		// get bit value at 1-3
		t = t >> (1-1);
		t = t & (1 << 3) -1;
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(t), "0000000000000011");
		t = 62139; //1111001010111011

		// get bit value at 9, 8, 7 (0-indexed) or 10,9,8 (1-indexed)
		t = t >> (8-1); // shift by 7 to make bit 7(zero based) the last bit or specify 8-1 to use 1-based index
		t = t & (1 << 3) -1; // unset all but last 3 bits
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(t), "0000000000000101");

		t = 62139; //1111001010111011
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(gamelib::BitFiddler<uint16_t>::GetBitsValue(t, 9, 3 )), "0000000000000101"); //62139 in binary
		
		t = 62139; // 1111001010111011
		// 0 and 1 (0-index) or 13,12 (1-indexed)
		t = t >> (1-1);
		t = t & (1 << 1)-1;
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(t), "0000000000000001");

		t = 62139; // 1111001010111011
		// 15 and 14 (0-index) or 13,12 (1-indexed)
		t = t >> (14-1);
		t = t & (1 << 1)-1;
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(t), "0000000000000001");	

		t = 62139; // 1111001010111011
		// 1 and 1
		t = t >> (1-1);
		t = t & (1 << 1)-1;
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(t), "0000000000000001");


		// get bit value at 9, 8, 7 (0-indexed) or 10,9,8 (1-indexed)
		//t = t >> (8-1); // shift by 7 to make bit 7(zero based) the last bit or specify 8-1 to use 1-based index
		//t = t & (1 << 3) -1; // unset all but last 3 bits

		uint8_t byte = 187; //10111011
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "10111011"); //144 in binary

		// get bits 6, 5, 4, 3
		byte = byte >> 3;
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "00010111"); //144 in binary
		//                                                       00000111
		//byte = byte & 7;
		byte = byte & (1 << 4) -1;
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "00000111");
		byte = 187;

		byte = 187; //10111011
		// get bits 3, 2, 1, 0
		byte = byte & (1 << 4) -1;
		EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(byte), "00001011");
	}

	TEST_F(BitFiddlerTests, SetBitsFirstPrinciplesTests)
	{
		constexpr uint16_t t = 62139; // 1111001010111011

		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(t), "1111001010111011");
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(std::numeric_limits<uint16_t>::max()),"1111111111111111");
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(std::numeric_limits<uint16_t>::max() >> 15),"0000000000000001"); // 16 - 1 = 1 bits to set
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(std::numeric_limits<uint16_t>::max() >> 14),"0000000000000011"); // 16 - 2 = 2 bits to set
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(std::numeric_limits<uint16_t>::max() >> 13),"0000000000000111"); // 16 - 3 = 3 bits to set
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(std::numeric_limits<uint16_t>::max() >> 12),"0000000000001111"); // 16 - 4 = 4 bits to set

		constexpr auto bitLength = 4;

		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString((std::numeric_limits<uint16_t>::max() >> (sizeof(uint16_t)*8) - bitLength) 
		<< (sizeof(uint16_t)*8) - bitLength),
		"1111000000000000");

		int startBit = 4;
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString((std::numeric_limits<uint16_t>::max() >> (sizeof(uint16_t)*8) - bitLength) 
		<< startBit-(bitLength-1)),
		"0000000000011110");

		startBit = 5;
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString((std::numeric_limits<uint16_t>::max() >> (sizeof(uint16_t)*8) - bitLength) 
		<< startBit-(bitLength-1)),
		"0000000000111100");

		startBit = 6;
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString((std::numeric_limits<uint16_t>::max() >> (sizeof(uint16_t)*8) - bitLength) 
		<< startBit-(bitLength-1)),
		"0000000001111000");


		uint16_t y = 0; // 0000000000000000

		uint16_t max = 0;
				max = ~(max & 0);

		// get a mask that sets all bits in interval concerned
		const uint16_t mask = (max >> (sizeof(uint16_t)*8) - bitLength) 	<< startBit-(bitLength-1);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(mask), "0000000001111000");

		// invert the mask so that that interval is all set to 0
		const uint16_t i_mask = ~mask;

		// apply mask to set the interval to all 0
		y &= i_mask;
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(y), "0000000000000000");

		// apply the value in the interval
		y = y | 5 << startBit-(bitLength-1);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(y), "0000000000101000");

		//uint8_t byte = 0; //00011000
		//EXPECT_EQ(gamelib::BitFiddler<uint8_t>::ToString(gamelib::BitFiddler<uint8_t>::SetBits(byte, 5, 3, 6 )), "00011000");
	}

	TEST_F(BitFiddlerTests, SetBiBitFieldTests)
	{                         // +-------+-------+-------+-------+
		uint16_t field = {0}; // |part1:4|part2:4|part3:6|part4:2|
		                      // +-------+-------+-------+-------+
		//                         0000    0000 0   00000   00

		//0101
		field = BitFiddler<uint16_t>::SetBits(field, 15, 4, 5);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(field), "0101000000000000");
		//0101 1111
		field = BitFiddler<uint16_t>::SetBits(field, 11, 4, 15);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(field), "0101111100000000");
		//0101 1111 000101
		field = BitFiddler<uint16_t>::SetBits(field, 7, 6, 5);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(field), "0101111100010100");
		//0101 1111 000101 10
		field = BitFiddler<uint16_t>::SetBits(field, 1, 2, 2);
		EXPECT_EQ(gamelib::BitFiddler<uint16_t>::ToString(field), "0101111100010110");

		uint32_t fullField = 0x0A0B0C0D; // 1010 0000 1011 0000 1100 0000 1101 (lets assume this is stored little endian)
		uint32_t field1 = 0x0A; // 1010 0000
		uint32_t field2 = 0x0B; // 1011 0000
		uint32_t field3 = 0x0C; // 1100 0000
		uint32_t field4 = 0x0D; // 1101 0000
	}
}
