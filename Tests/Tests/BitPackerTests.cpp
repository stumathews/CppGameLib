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

TEST_F(BitPackingTests, Basic)
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
	BitPacker<uint16_t> bitPacker;

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


	auto t1 = BITS_REQUIRED(0,2);
	EXPECT_EQ(t1, 2);
	auto t2 = BITS_REQUIRED(0,5);
	EXPECT_EQ(t2, 3);
	auto t3 = BITS_REQUIRED(0,15);
	EXPECT_EQ(t3, 4);
	auto t4 = BITS_REQUIRED(0,5);
	EXPECT_EQ(t4, 3);
	
	

	// pack a bitfield backwards encoded as  num1:4|num2:4|num3:6|num4:2

	// must pack in this order

	bitPacker.Pack(BITS_REQUIRED(0,2), a.Num4);  // 0101 
	bitPacker.Pack(BITS_REQUIRED(0,5), a.Num3);  // 000101
	bitPacker.Pack(BITS_REQUIRED(0,15), a.Num2); // 1111
	bitPacker.Pack(BITS_REQUIRED(0,5), a.Num1);  // 0101

	// must unpack in this order
	a.Num4 = bitPacker.UnPack(BITS_REQUIRED(0,2)); EXPECT_EQ(a.Num4, 2);	
	a.Num3 = bitPacker.UnPack(BITS_REQUIRED(0,5)); EXPECT_EQ(a.Num3, 5);	
	a.Num2 = bitPacker.UnPack(BITS_REQUIRED(0,15)); EXPECT_EQ(a.Num2, 15);
	a.Num1 = bitPacker.UnPack(BITS_REQUIRED(0,5)); EXPECT_EQ(a.Num1, 5);	

	EXPECT_EQ(BitFiddler<uint16_t>::ToString(bitPacker.GetBuffer()), "0000101111110110");
	
	
}
