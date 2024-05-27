#include "pch.h"
#include "utils/BitFiddler.h"
#include "net/BitPacker.h"

using namespace gamelib;

class BitPackingTests : public testing::Test
{
public:	

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(BitPackingTests, BasicBitPacking)
{
	constexpr int MaxElements = 32;

	struct PacketB
	{
	    int numElements; // ideally the network sent only 6 bits to represent any number up to 63 (5 bits would be up to 31)
	    int elements[MaxElements]; // idealy the network sent

	    //void Write(gamelib::BitWriter & writer )
	    //{
		   // WriteBits( writer, numElements, 6 ); // Write the 6 bits from network into numElements
	    //    for ( int i = 0; i < numElements; ++i )
		   //     WriteBits( writer, elements[i] );
	    //}

	    //void Read(gamelib::BitReader & reader )
	    //{
	    //    ReadBits( reader, numElements, 6 );
	    //    for ( int i = 0; i < numElements; ++i )
	    //        ReadBits( reader, elements[i] );
	    //}
	};

	uint16_t output[2] {0};

	BitPacker<uint16_t> bitPacker(output,2);

	#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

	struct Dummy
	{
		uint16_t Num1; // 3 bits can store up to value 5 (inclusive)
		uint16_t Num2; // 4 bits can store up to value 15 (inclusive)
		uint16_t Num3; // 3 bits can store up to value 5 (inclusive)
		uint16_t Num4; // 2 bits can store up to value 2 (inclusive)
	} a {};

	a.Num4 = 2;
	a.Num3 = 5;
	a.Num2 = 15;
	a.Num1 = 5;

	// check how many bits we need to store the maximum value we're planning to store in the data structire

	constexpr auto t1 = BITS_REQUIRED(0,2);
	EXPECT_EQ(t1, 2);
	constexpr auto t2 = BITS_REQUIRED(0,5);
	EXPECT_EQ(t2, 3);
	constexpr auto t3 = BITS_REQUIRED(0,15);
	EXPECT_EQ(t3, 4);
	constexpr auto t4 = BITS_REQUIRED(0,5);
	EXPECT_EQ(t4, 3);
	
	// pack a bitfield backwards encoded as  num1:4|num2:4|num3:6|num4:2

	// must pack in this order

	bitPacker.Pack(BITS_REQUIRED(0,2), a.Num4);  // 10 
	bitPacker.Pack(BITS_REQUIRED(0,5), a.Num3);  // 101
	bitPacker.Pack(BITS_REQUIRED(0,15), a.Num2); // 1111
	bitPacker.Pack(BITS_REQUIRED(0,5), a.Num1);  // 101
	bitPacker.Flush();

	// must unpack in this order
	BitfieldReader fieldReader(output);

	a.Num4 = fieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0,2)); EXPECT_EQ(a.Num4, 2);	
	a.Num3 = fieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0,5)); EXPECT_EQ(a.Num3, 5);	
	a.Num2 = fieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0,15)); EXPECT_EQ(a.Num2, 15);
	a.Num1 = fieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0,5)); EXPECT_EQ(a.Num1, 5);	

	EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[0]), "0000101111110110");

	bitPacker.Reset();

	// lets pack all numbers as 4 bits instead of uneven sizes
	const auto t5 = BITS_REQUIRED(0,15);
	EXPECT_EQ(t5, 4);

	// this does however mean that we can only set each field up to value 15 (inclusive)
	// We will use the original values, which will just now span 4 bits each
	bitPacker.Pack(BITS_REQUIRED(0,15), a.Num4); // 0010 
	bitPacker.Pack(BITS_REQUIRED(0,15), a.Num3); // 0101
	bitPacker.Pack(BITS_REQUIRED(0,15), a.Num2); // 1111
	bitPacker.Pack(BITS_REQUIRED(0,15), a.Num1); // 0101

	EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[0]), "0101111101010010");	
}

TEST_F(BitPackingTests, OverflowTests)
{
	uint16_t output[3] {0};

	#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

	struct Dummy
	{
		uint16_t Num1; // 3 bits can store up to value 5 (inclusive)
		uint16_t Num2; // 4 bits can store up to value 15 (inclusive)
		uint16_t Num3; // 3 bits can store up to value 5 (inclusive)
		uint16_t Num4; // 2 bits can store up to value 2 (inclusive)
	} a {};

	a.Num4 = 2;
	a.Num3 = 5;
	a.Num2 = 15;
	a.Num1 = 5;

	BitPacker bitPacker(output, 3);

	bitPacker.Pack(BITS_REQUIRED(0,20), a.Num4); // 00010 
	bitPacker.Pack(BITS_REQUIRED(0,20), a.Num3); // 00101
	bitPacker.Pack(BITS_REQUIRED(0,20), a.Num2); // 01111
	bitPacker.Pack(BITS_REQUIRED(0,20), a.Num1); // 00101 // only first bit(right most bit) can fit, leaves 0010 to overflow into overflow buffer

	// check that that we've got the correct 16 bits
	EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[0]), "1011110010100010");

	// pack 12 more bits so we dump out the next 16bits to output buffer
	bitPacker.Pack(12,4095); // 111111111111	

	// check the overflow buffer
	EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[1]), "1111111111110010");  //make sure 111111111111 is appended to the 4 overflowed bits, 0010

	// Get the the bits as a 32bit block
	uint32_t all {0};

	// copy 32bits that are in the output to all
	memcpy_s(&all, sizeof(uint32_t), output, sizeof(uint32_t));

	// ensure that its the same as 2 individual 16bit blocks, and that they are contiguous
	EXPECT_EQ(BitFiddler<uint32_t>::ToString(all), "11111111111100101011110010100010");

	// Pack another 16bits in
	bitPacker.Pack(16,65535);

	// and check that the output buffer is growing accordingly
	EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[2]), "1111111111111111");

	EXPECT_EQ(bitPacker.TotalBitsPacked(), 48);
	
}

TEST_F(BitPackingTests, BitfieldReaderTests)
{
	uint16_t output[3] {0}; // 48bit block

	#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

	constexpr int MaxElements = 3;

	struct PacketB
	{
	    uint16_t numElements; // max is 6 elements;
	    uint16_t elements[MaxElements]; // each item can store a value from 0-255
	} packet {};

	packet.numElements = 6; //110
	packet.elements[0] = 10; //00001010
	packet.elements[1] = 9; //00001001
	packet.elements[2] = 1; //00000001

	// check how many bits the values can fit into
	constexpr auto t1 = BITS_REQUIRED( 0,6 );
	constexpr auto t2 = BITS_REQUIRED( 0,255 );
	EXPECT_EQ(t1, 3);
	EXPECT_EQ(t2, 8);

	// Prepare our bitpacker to output its packed bits to an output buffer
	BitPacker bitPacker(output, 3);

	bitPacker.Pack( BITS_REQUIRED( 0,6 ), packet.numElements );
	bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.elements[0]);
	bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.elements[1]);
	bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.elements[2]);
	bitPacker.Pack(5, 31); //11111

	EXPECT_EQ( BitFiddler<uint16_t>::ToString(output[0]), "0100100001010110");
	EXPECT_EQ( BitFiddler<uint16_t>::ToString(output[1]), "1111100000001000");
	
	EXPECT_EQ( bitPacker.TotalBitsPacked(), 32);

	uint32_t all {0};

	// copy 32bits that are in the output to all
	memcpy_s(&all, sizeof(uint32_t), output, sizeof(uint32_t));

	EXPECT_EQ(BitFiddler<uint32_t>::ToString(all), "11111000000010000100100001010110");
		
	BitfieldReader reader(&all);

	EXPECT_EQ(reader.ReadNext<uint8_t>(3),6);  EXPECT_EQ(reader.ReadInterval<uint8_t>(2,3),6);
	EXPECT_EQ(reader.ReadNext<uint8_t>(8),10); EXPECT_EQ(reader.ReadInterval<uint8_t>(10,8),10);
	EXPECT_EQ(reader.ReadNext<uint8_t>(8),9);  EXPECT_EQ(reader.ReadInterval<uint8_t>(18,8),9);
	EXPECT_EQ(reader.ReadNext<uint8_t>(8),1);  EXPECT_EQ(reader.ReadInterval<uint8_t>(26,8),1);
	EXPECT_EQ(reader.ReadNext<uint8_t>(5),31); EXPECT_EQ(reader.ReadInterval<uint8_t>(31,5),31);

	reader.Reset();

	packet.numElements = reader.ReadNext<uint8_t>(3); //110
	packet.elements[0] = reader.ReadNext<uint8_t>(8); //00001010
	packet.elements[1] = reader.ReadNext<uint8_t>(8); //00001001
	packet.elements[2] = reader.ReadNext<uint8_t>(8); //00000001

	EXPECT_EQ(packet.numElements,6);
	EXPECT_EQ(packet.elements[0],10);
	EXPECT_EQ(packet.elements[1] ,9);
	EXPECT_EQ(packet.elements[2],1);
	
}
