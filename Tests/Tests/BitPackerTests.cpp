#include "pch.h"
#include "TestData.h"
#include <utils/BitFiddler.h>
#include <net/BitPacker.h>

#include "gtest/gtest.h"

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
	BitfieldReader fieldReader(output, 2);

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
	
	//struct PacketB
	//{
	//    uint16_t numElements; // max is 6 elements;
	//    uint16_t elements[3]; // each item can store a value from 0-255
	//} packet {};

	TestData::TestNetworkPacket packet {};

	packet.NumElements = 6; //110
	packet.Elements[0] = 10; //00001010
	packet.Elements[1] = 9; //00001001
	packet.Elements[2] = 1; //00000001

	// check how many bits the values can fit into
	constexpr auto t1 = BITS_REQUIRED( 0,6 );
	constexpr auto t2 = BITS_REQUIRED( 0,255 );
	EXPECT_EQ(t1, 3);
	EXPECT_EQ(t2, 8);

	// Prepare our bitpacker to output its packed bits to an output buffer
	BitPacker bitPacker(output, 3);

	bitPacker.Pack( BITS_REQUIRED( 0,6 ), packet.NumElements );
	bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.Elements[0]);
	bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.Elements[1]);
	bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.Elements[2]);
	bitPacker.Pack(5, 31); //11111

	EXPECT_EQ( BitFiddler<uint16_t>::ToString(output[0]), "0100100001010110");
	EXPECT_EQ( BitFiddler<uint16_t>::ToString(output[1]), "1111100000001000");
	
	EXPECT_EQ( bitPacker.TotalBitsPacked(), 32);

	uint32_t all {0};

	// copy 32bits that are in the output to all
	memcpy_s(&all, sizeof(uint32_t), output, sizeof(uint32_t));

	EXPECT_EQ(BitFiddler<uint32_t>::ToString(all), "11111000000010000100100001010110");
		
	BitfieldReader reader(&all, 1);

	EXPECT_EQ(reader.ReadNext<uint8_t>(3),6);  EXPECT_EQ(reader.ReadInterval<uint8_t>(2,3),6);
	EXPECT_EQ(reader.ReadNext<uint8_t>(8),10); EXPECT_EQ(reader.ReadInterval<uint8_t>(10,8),10);
	EXPECT_EQ(reader.ReadNext<uint8_t>(8),9);  EXPECT_EQ(reader.ReadInterval<uint8_t>(18,8),9);
	EXPECT_EQ(reader.ReadNext<uint8_t>(8),1);  EXPECT_EQ(reader.ReadInterval<uint8_t>(26,8),1);
	EXPECT_EQ(reader.ReadNext<uint8_t>(5),31); EXPECT_EQ(reader.ReadInterval<uint8_t>(31,5),31);

	reader.Reset();

	packet.NumElements = reader.ReadNext<uint8_t>(3); //110
	packet.Elements[0] = reader.ReadNext<uint8_t>(8); //00001010
	packet.Elements[1] = reader.ReadNext<uint8_t>(8); //00001001
	packet.Elements[2] = reader.ReadNext<uint8_t>(8); //00000001

	EXPECT_EQ(packet.NumElements,6);
	EXPECT_EQ(packet.Elements[0],10);
	EXPECT_EQ(packet.Elements[1] ,9);
	EXPECT_EQ(packet.Elements[2],1);
	
}

TEST_F(BitPackingTests, BitfieldReaderWithOVerflowSupport)
{
	

	constexpr auto t1 = BITS_REQUIRED( 0,3 );
	constexpr auto t2 = BITS_REQUIRED( 0,255 );
	EXPECT_EQ(t1, 2);
	EXPECT_EQ(t2, 8);

	TestData::TestNetworkPacket packetB;

	packetB.NumElements = 3; // 11
	packetB.Elements[0] = 10;// 00001010
	packetB.Elements[1] = 9; // 00001001
	packetB.Elements[2] = 1; // 00000001

	// 00000001 000010010000101011

	uint16_t networkBuffer[4] = {0}; // 64 byte network buffer

	BitPacker bitPacker(networkBuffer, 3); // packer of 16bits at a time

	// Write to networkBuffer via bitPacker
	packetB.Write(bitPacker);

	//bitPacker.Pack(6, 63); //111111 (make it auto-flush at we've no got 16 more bits)
	//bitPacker.Flush();

	//1111110000000100 0010010000101011
	
	EXPECT_EQ(BitFiddler<uint16_t>::ToString(networkBuffer[0]), "0010010000101011");
	EXPECT_EQ(BitFiddler<uint16_t>::ToString(networkBuffer[1]), "0000000000000100");

	// RHS               LHS
	// 11111100000001_00 001001_00001010_11
	//              1         9       10  3
		
	EXPECT_EQ(bitPacker.TotalBitsPacked(), 26);

	BitfieldReader reader(networkBuffer, 3);

	EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 3 )), 3);
	EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 255 )), 10);
	EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 255 )), 9); // overflows here
	EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 255 )), 1); 
	//EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 63 )), 63);
	
	reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 63 ));
	reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 63 ));
	reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 63 ));
	reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 63 ));

	// Should detect when we are trying to read beyond the bound of the underlying buffer
	EXPECT_THROW(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 63 )), std::exception);

}

TEST_F(BitPackingTests, PacketReadingWriting)
{
	#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

	constexpr int maxElements = 3;
	constexpr int numElements = maxElements;
	constexpr int element0 = 200;
	constexpr int element1 = 254;
	constexpr int element2 = 199;

	struct Packet
	{
	    int NumElements;
	    int Elements[maxElements];

	    void Write(BitPacker<uint16_t>& bitPacker) const
	    {
			bitPacker.Pack(BITS_REQUIRED( 0,maxElements ), NumElements);
			for(int i = 0; i < NumElements;i++)
			{
				bitPacker.Pack(BITS_REQUIRED( 0,255 ), Elements[i]);
			}
			bitPacker.Flush();
	    }

	    void Read(BitfieldReader<uint16_t>& bitfieldReader)
	    {
	        NumElements = bitfieldReader.ReadNext<int>(BITS_REQUIRED( 0,maxElements ));
			for(int i = 0; i < NumElements;i++)
			{
				Elements[i] = bitfieldReader.ReadNext<int>(BITS_REQUIRED( 0,255 ));
			}
	    }
	};

	// This is where we'll write out packet to before sending.
	uint16_t networkBuffer[2] = {0}; // 32bit buffer

	// This is our bit packer
	BitPacker bitPacker(networkBuffer, 2);

	// This is our bitfieldReader
	BitfieldReader reader(networkBuffer, 2);

	// Make a packet and fill it in...
	Packet packetA {};

	packetA.NumElements = numElements;
	packetA.Elements[0] = element0;
	packetA.Elements[1] = element1;
	packetA.Elements[2] = element2;

	// write to network buffer via BitPacker
	packetA.Write(bitPacker);

	// make a new packet, eg. reconstruct it from the networkBuffer via bitFieldReader
	Packet packetB {};

	packetB.Read(reader);
	EXPECT_EQ(packetB.NumElements, numElements);
	EXPECT_EQ(packetB.Elements[0], element0);
	EXPECT_EQ(packetB.Elements[1], element1);
	EXPECT_EQ(packetB.Elements[2], element2);
}

TEST_F(BitPackingTests, SimulateNetworkingUsage)
{
	char buffer[3];

	BitPacker bitPacker(buffer, 2);

	const uint8_t minBitsFor0 = BITS_REQUIRED(0, 3); // 2
	const uint8_t minBitsFor6 = BITS_REQUIRED(0, 6); // 3
	const uint8_t minBitsFor9 = BITS_REQUIRED(0, 9); // 4

	bitPacker.Pack(minBitsFor0, 0); // 0
	bitPacker.Pack(minBitsFor6, 6); //0110
	bitPacker.Pack(minBitsFor9, 9); // 1001
	bitPacker.Flush();

	// 001101001

	auto show1 = BitFiddler<char>::ToString(buffer[0]); // 00000001 001_110_00

	const double numBytes = static_cast<double>(bitPacker.TotalBitsPacked()) / static_cast<double>(8);
	const auto expectedBytesSent = ceil(numBytes);

	auto receivedBuffer = reinterpret_cast<char*>(buffer);
	auto receivedBufferSize = 2;

	EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[0]), "00111000");
	EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[1]), "00000001");

	BitfieldReader<char> bitFieldReader(receivedBuffer, receivedBufferSize);

	EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor0), 0);
	EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor6), 6);
	EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor9), 9);

}

TEST_F(BitPackingTests, BitPackingTypes_String)
{	
	// Number of units in the buffer
	constexpr int numBufferUnits = 256; // enough for 512 character string

	// Pack bits into this buffer:
	uint16_t destBuffer[numBufferUnits]{};		

	// This is our bit-packer we're going to use
	auto bitPacker = BitPacker(destBuffer, numBufferUnits);

	// This is our Bit reader
	auto bitReader = BitfieldReader(destBuffer, numBufferUnits);

	// This is the string we want to pack into bit-packer
	const auto sampleString = "Hello world, I'm back!! My name is arnie shawaszenegger";

	// This is our String type we're going to serialize/deserialize
	bit_packing_types::String string = sampleString;

	// lets serialize to buffer
	string.Write(bitPacker, sampleString);

	bitPacker.Finish(); // after writing to the bit-packer make sure it has flushed everything to the buffer

	// lets deserialize to into another packet
	bit_packing_types::String anotherPacket {};
	anotherPacket.Read(bitReader);

	EXPECT_STREQ(anotherPacket.c_str(), sampleString);
}

TEST_F(BitPackingTests, BitPackingStrings)
{
	// This test packs a two strings 

	struct Person
	{
		bit_packing_types::String Name;
		bit_packing_types::String Surname;
		int Age {};

		Person() : Person("","",0) 	{ }

		Person(const std::string& name, const std::string& surname, const int age)
		{
			Name.Initialize(name);
			Surname.Initialize(surname);
			Age = age;
		}		

		void Write(BitPacker<uint16_t>& bitPacker)
		{			
			Name.Write(bitPacker, Name.c_str());
			Surname.Write(bitPacker, Surname.c_str());
			bitPacker.Pack(BITS_REQUIRED(1,100), Age); // person can only be aged between 1 and 100
		}

		void Read(BitfieldReader<uint16_t>& bitfieldReader)
	    {
			// Read in the same order as when writing
			Name.Read(bitfieldReader);
			Surname.Read(bitfieldReader);
			Age = bitfieldReader.ReadNext<int>(BITS_REQUIRED(1,100));

		}
	};
	
	// Number of units in the buffer
	constexpr int numBufferUnits = 256; // enough for 512 character string

	// Pack bits into this buffer:
	uint16_t destBuffer[numBufferUnits]{};	
	
	// This is our bitpacker we're going to use
	auto bitPacker = BitPacker(destBuffer, numBufferUnits);

	// This is our Bit reader
	auto bitReader = BitfieldReader(destBuffer, numBufferUnits);

	// This is our packet we're going to serialize/deserialize
	const auto name = "Stuart";
	const auto surname = "Mathews";
	constexpr auto age = 40;

	Person person1(name, surname, age);

	// lets serialize to buffer
	person1.Write(bitPacker);

	bitPacker.Finish(); // once finished with bit packer flush it to make sure its all sent to buffer

	// lets deserialize to into another packet
	Person person2 {};
	person2.Read(bitReader);

	EXPECT_STREQ(person1.Name.c_str(), person2.Name.c_str());
	EXPECT_STREQ(person1.Surname.c_str(), person2.Surname.c_str());
	EXPECT_EQ(person1.Age, person2.Age);
	
	





}