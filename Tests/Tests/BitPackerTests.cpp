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

	uint16_t packetBuffer = 62139; // 111 1001010111 011
	BitWriter writer(sizeof(uint16_t), &packetBuffer);

	PacketB newPacket {};

	writer.Write(3, static_cast<unsigned short*>((void*)&newPacket));

	//EXPECT_EQ(newPacket.numElements, 7);
	
}
