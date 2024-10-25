#include "pch.h"
#include "TestData.h"
#include <utils/BitFiddler.h>
#include <net/BitPacker.h>
#include "gtest/gtest.h"
#include "net/BitPackingTypes.h"
#include "net/PacketDatum.h"

namespace gamelib
{

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
		#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

		uint16_t output[2] {0}; // 32-bit buffer

		BitPacker bitPacker(output,2);

		struct Dummy
		{
			uint16_t Num1; // 3 bits can store up to value 5 (inclusive)
			uint16_t Num2; // 4 bits can store up to value 15 (inclusive)
			uint16_t Num3; // 3 bits can store up to value 5 (inclusive)
			uint16_t Num4; // 2 bits can store up to value 2 (inclusive)
		} a {};
		
		// check how many bits we need to store the maximum value we're planning to store in the data structure: 12bits

		constexpr auto t1 = BITS_REQUIRED(0,2);
		constexpr auto t2 = BITS_REQUIRED(0,5);	
		constexpr auto t3 = BITS_REQUIRED(0,15);	
		constexpr auto t4 = BITS_REQUIRED(0,5);
		
		EXPECT_EQ(t1, 2);
		EXPECT_EQ(t2, 3);
		EXPECT_EQ(t3, 4);
		EXPECT_EQ(t4, 3);

		// These are the values we'll be packing
		
		a.Num4 = 2;
		a.Num3 = 5;
		a.Num2 = 15;
		a.Num1 = 5;

		// pack a bitfield backwards encoded as  num1:2|num2:15|num3:5|num4:2

		// must pack in this order

		bitPacker.Pack(BITS_REQUIRED(0,2), a.Num4);  // 10 is 2 (2 bits)
		bitPacker.Pack(BITS_REQUIRED(0,5), a.Num3);  // 101 is 5 (3 bits)
		bitPacker.Pack(BITS_REQUIRED(0,15), a.Num2); // 1111 is 15 (4 bits)
		bitPacker.Pack(BITS_REQUIRED(0,5), a.Num1);  // 101 is 5 (3 bits)

		EXPECT_EQ(bitPacker.BitsLeftInCurrentSegment(), 4); // should be 4 bits out of 16 left
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 12);

		bitPacker.Flush();

		EXPECT_EQ(bitPacker.TotalBitsPacked(), 16);
		EXPECT_EQ(bitPacker.TotalSegmentsWritten(), 1); // we flushed 1 segment to memory
		EXPECT_EQ(bitPacker.BitsLeftInCurrentSegment(), 16); // we flushed, so there should be the full 16 bits left in the new segment

		// must unpack in this order
		BitfieldReader fieldReader(output, 2); //ink reader to 32-bit buffer

		// Read values out of buffer by reading as many bits are required to get the min-max bits that the value can fall into
		a.Num4 = fieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0,2)); EXPECT_EQ(a.Num4, 2);	
		a.Num3 = fieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0,5)); EXPECT_EQ(a.Num3, 5);	
		a.Num2 = fieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0,15)); EXPECT_EQ(a.Num2, 15);
		a.Num1 = fieldReader.ReadNext<uint16_t>(BITS_REQUIRED(0,5)); EXPECT_EQ(a.Num1, 5);	

		// We only read 12 bits
		EXPECT_EQ(fieldReader.TotalBitsRead(), 12);

		// Check that after reading 12 bits of a 16bit segment, there are 4 unused bits left over
		EXPECT_EQ(fieldReader.BitsLeftInSegment(), 4);

		// Visually inspect result
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[0]), "0000101111110110");

		// set all pointers, accumulators back to zero and lets re-use the reader again
		bitPacker.Reset();

		// lets pack all numbers as 4 bits instead of uneven sizes...

		// this does however mean that we can only set each field up to value 15 (inclusive)
		// We will use the original values, which will just now span 4 bits each
		bitPacker.Pack(BITS_REQUIRED(0,15), a.Num4); // 0010 
		bitPacker.Pack(BITS_REQUIRED(0,15), a.Num3); // 0101
		bitPacker.Pack(BITS_REQUIRED(0,15), a.Num2); // 1111
		bitPacker.Pack(BITS_REQUIRED(0,15), a.Num1); // 0101

		EXPECT_EQ(bitPacker.TotalBitsPacked(), 16);
		EXPECT_EQ(bitPacker.TotalSegmentsWritten(), 1); // we flushed 1 segment to memory
		EXPECT_EQ(bitPacker.BitsLeftInCurrentSegment(), 16); // should be 4 bits out of 16 left
		
		// Visuall inspect result
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[0]), "0101111101010010");	
	}

	TEST_F(BitPackingTests, OverflowTests)
	{

		#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

		struct Dummy
		{
			uint16_t Num1; // 3 bits can store up to value 5 (inclusive)
			uint16_t Num2; // 4 bits can store up to value 15 (inclusive)
			uint16_t Num3; // 3 bits can store up to value 5 (inclusive)
			uint16_t Num4; // 2 bits can store up to value 2 (inclusive)
		} dummy {};

		dummy.Num4 = 2;
		dummy.Num3 = 5;
		dummy.Num2 = 15;
		dummy.Num1 = 5;

		
		uint16_t output[3] {0}; // 48-bit buffer

		BitPacker bitPacker(output, 3); // 16-bit segment packer

		// we'll use 4 bits each time for a value between 0-20
		constexpr auto t20 = BITS_REQUIRED(0,20);
		EXPECT_EQ(t20, 5);

		bitPacker.Pack(BITS_REQUIRED(0,20), dummy.Num4); // 00010 
		bitPacker.Pack(BITS_REQUIRED(0,20), dummy.Num3); // 00101
		bitPacker.Pack(BITS_REQUIRED(0,20), dummy.Num2); // 01111
		bitPacker.Pack(BITS_REQUIRED(0,20), dummy.Num1); // 00101 // only first bit(right most bit) can fit, leaves 0010 to overflow into overflow buffer

		EXPECT_EQ(bitPacker.TotalBitsPacked(), 20);
		EXPECT_EQ(bitPacker.TotalSegmentsWritten(), 1);
		EXPECT_EQ(bitPacker.BitsLeftInCurrentSegment(), 12); // 12 bits left in 16bit segment

		// check that that we've got the correct 16 bits
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[0]), "1011110010100010");

		// pack 12 more bits so we dump out the next 16bits to output buffer
		bitPacker.Pack(12,4095); // 111111111111	
		EXPECT_EQ(bitPacker.TotalSegmentsWritten(), 2); // it correctly overflowed
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 32);

		// check the overflow buffer
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[1]), "1111111111110010");  //make sure 111111111111 is appended to the 4 overflowed bits, 0010

		// Get the the bits as a single 32bit block
		uint32_t all {0};

		// copy 32bits that are in the output to all
		memcpy_s(&all, sizeof(uint32_t), output, sizeof(uint32_t));

		// ensure that its the same as 2 individual 16bit blocks, and that they are contiguous
		EXPECT_EQ(BitFiddler<uint32_t>::ToString(all), "11111111111100101011110010100010");

		// Pack another 16bits in
		bitPacker.Pack(16,65535);
		EXPECT_EQ(bitPacker.TotalSegmentsWritten(),3);
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 48);

		// and check that the output buffer is growing accordingly
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(output[2]), "1111111111111111");

		EXPECT_EQ(bitPacker.TotalBitsPacked(), 48);	
	}

	TEST_F(BitPackingTests, BitfieldReaderTests)
	{

		#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result
		
		TestData::TestNetworkPacket packet {};

		// check how many bits the values can fit into
		constexpr auto t1 = BITS_REQUIRED( 0,6 );
		constexpr auto t2 = BITS_REQUIRED( 0,255 );
		EXPECT_EQ(t1, 3);
		EXPECT_EQ(t2, 8);

		packet.NumElements = 6; //110 (3 bits can be used)
		packet.Elements[0] = 10; //00001010 (8 bits will be used, see above)
		packet.Elements[1] = 9; //00001001
		packet.Elements[2] = 1; //00000001	

		uint16_t output[3] {0}; // 48bit block

		// Prepare our 16-bit bit-packer to output its packed bits to an output buffer
		BitPacker bitPacker(output, 3);

		bitPacker.Pack( BITS_REQUIRED( 0,6 ), packet.NumElements ); // 3 bits packed
		bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.Elements[0]); // 8 bits packed 
		bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.Elements[1]); // ...
		bitPacker.Pack( BITS_REQUIRED( 0,255 ), packet.Elements[2]); //.. 8
		bitPacker.Pack(5, 31); //back the value 31 (5 bits): 11111

		EXPECT_EQ( BitFiddler<uint16_t>::ToString(output[0]), "0100100001010110"); //0100100001010110
		EXPECT_EQ( BitFiddler<uint16_t>::ToString(output[1]), "1111100000001000");
		//                                                     0111110000000100
		
		EXPECT_EQ( bitPacker.TotalBitsPacked(), 32); 
		EXPECT_EQ(bitPacker.TotalSegmentsWritten(), 2); // 2 16-bit writes should result in 2 segment counts written

		uint32_t all {0};

		// copy 32bits that are in the output to all
		memcpy_s(&all, sizeof(uint32_t), output, sizeof(uint32_t));

		EXPECT_EQ(BitFiddler<uint32_t>::ToString(all), "11111000000010000100100001010110");
			
		BitfieldReader reader(&all, 1); // We have a 23-bit reader now

		// reader first 3 bits, should result in the value 6 
		EXPECT_EQ(reader.ReadNext<uint8_t>(3),6);

		 // That should be stored in the bit range of high bit 2 for (3 bits), i.e bits 2,1,0 (all inclusive)
		EXPECT_EQ(reader.ReadInterval<uint8_t>(2,3),6);

		// read next 8 bits, that should result in the value 10
		EXPECT_EQ(reader.ReadNext<uint8_t>(8),10);

		// that should be stored in the bits 10, 9, 8, 7, 6, 5, 4, 3  (8 bits) (all inclusive)
		EXPECT_EQ(reader.ReadInterval<uint8_t>(10,8),10);

		// read next 8 bits, that should result in value 9
		EXPECT_EQ(reader.ReadNext<uint8_t>(8),9);

		// that should be stored in the interval 18, 17, 16, 15, 14, 13, 12, 11 (bit bits)
		EXPECT_EQ(reader.ReadInterval<uint8_t>(18,8),9);

		// Read next 8 bits, that should result in value 1
		EXPECT_EQ(reader.ReadNext<uint8_t>(8),1);

		// That value should fall in the bit interval of 26, 25, 24, 23, 22, 21, 20, 19
		EXPECT_EQ(reader.ReadInterval<uint8_t>(26,8),1);

		// Read next 5 bits, that should result in the value 31
		EXPECT_EQ(reader.ReadNext<uint8_t>(5),31);

		// 31 should come from the bits in the interval of bits 31, 30, 29, 28, 27
		EXPECT_EQ(reader.ReadInterval<uint8_t>(31,5),31);

		// Reset bit position in the reader/stream to 0
		reader.Reset();

		// Reread the same bits again and sensire the same values some out
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
		constexpr auto t3 = BITS_REQUIRED( 0,63 );

		EXPECT_EQ(t1, 2); // values 0-3 can be stored in 2 bits
		EXPECT_EQ(t2, 8); // values 0-255 can be stored in 8 bits
		EXPECT_EQ(t3, 6); // values 0-63 can be stored in 6 bits

		TestData::TestNetworkPacket packetB{};

		packetB.NumElements = 3; // 11
		packetB.Elements[0] = 10;// 00001010
		packetB.Elements[1] = 9; // 00001001
		packetB.Elements[2] = 1; // 00000001

		// 00000001000010010000101011 (26bits right to left)
		//           ^  
		uint16_t networkBuffer[4] = {0}; // 64 byte network buffer

		BitPacker bitPacker(networkBuffer, 4); // 16-bit packer

		// Write to networkBuffer via bitPacker
		packetB.Write(bitPacker); // Should pack 2 + 3*8 bits = 26 bits written
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 26);
		EXPECT_EQ(bitPacker.TotalSegmentsWritten(), 1);
		EXPECT_EQ(bitPacker.BitsLeftInCurrentSegment(), 6);

		bitPacker.Finish(); // write the current segment (including the 6 bits left) to the buffer, that makes 32 bits written
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 32);
		EXPECT_EQ(bitPacker.BitsLeftInCurrentSegment(), 16);
		EXPECT_EQ(bitPacker.TotalSegmentsWritten(), 2);

		//0000000000000100 0010010000101011
		
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(networkBuffer[0]), "0010010000101011"); // 16bits
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(networkBuffer[1]), "0000000000000100"); // 16bits

		bitPacker.Pack(6, 63); //111111 (make it auto-flush at we've no got 16 more bits)
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 38);

		bitPacker.Finish();
		//0000000000111111 0000000000000100 0010010000101011

		EXPECT_EQ(BitFiddler<uint16_t>::ToString(networkBuffer[2]), "0000000000111111");
			
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 48); // out of 64 total bits

		BitfieldReader reader(networkBuffer, 4);

		EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 3 )), 3); // read 2 bits
		EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 255 )), 10); //read 8 bits
		EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 255 )), 9); // read 8 bits overflows here
		EXPECT_EQ(reader.TotalBitsRead(), 18);
		EXPECT_EQ(reader.BitsLeftInSegment(), 14);
		EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 255 )), 1);
		EXPECT_EQ(reader.BitsLeftInSegment(), 6);
		EXPECT_EQ(reader.TotalBitsRead(), 26);

		// flush called here
		reader.Finish();

		EXPECT_EQ(reader.TotalBitsRead(), 32);
		EXPECT_EQ(reader.BitsLeftInSegment(), 16);
		EXPECT_EQ(reader.SegmentsRead(), 2);
		EXPECT_EQ(reader.ReadNext<uint8_t>(BITS_REQUIRED( 0, 63 )), 63);
		
		// flush called here
		reader.Finish();
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 48);
		
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
		packetA.Write(bitPacker); bitPacker.Finish();

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

		constexpr uint8_t minBitsFor0 = BITS_REQUIRED(0, 3); // 2
		constexpr uint8_t minBitsFor6 = BITS_REQUIRED(0, 6); // 3
		constexpr uint8_t minBitsFor9 = BITS_REQUIRED(0, 9); // 4

		bitPacker.Pack(minBitsFor0, 0); // 0
		bitPacker.Pack(minBitsFor6, 6); //0110
		bitPacker.Pack(minBitsFor9, 9); // 1001
		bitPacker.Flush();

		// 001101001

		auto show1 = BitFiddler<char>::ToString(buffer[0]); // 00000001 001_110_00

		const double numBytes = static_cast<double>(bitPacker.TotalBitsPacked()) / static_cast<double>(8);
		const auto expectedBytesSent = ceil(numBytes);

		const auto receivedBuffer = reinterpret_cast<char*>(buffer);
		constexpr auto receivedBufferSize = 2;

		EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[0]), "00111000");
		EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[1]), "00000001");

		BitfieldReader bitFieldReader(receivedBuffer, receivedBufferSize);

		EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor0), 0);
		EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor6), 6);
		EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor9), 9);

	}

	TEST_F(BitPackingTests, PushBuffer)
	{
		uint32_t buffer[512];  // 16kib
		BitPacker packer(buffer, 512);

		constexpr auto maxBitsPerNumber = BITS_REQUIRED( 0, 200);

		packer.Pack(maxBitsPerNumber, 98);
		packer.Pack(maxBitsPerNumber, 99);
		packer.Pack(maxBitsPerNumber, 100);

		const auto stuart = "Stuart";

		packer.PushBytes(stuart, strlen(stuart));

		packer.Finish();

		BitfieldReader reader(buffer, 512);

		EXPECT_EQ(reader.ReadNext<int>(maxBitsPerNumber), 98);
		EXPECT_EQ(reader.ReadNext<int>(maxBitsPerNumber), 99);
		EXPECT_EQ(reader.ReadNext<int>(maxBitsPerNumber), 100);
	}

	TEST_F(BitPackingTests, OneLongString)
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
		const auto sampleString = "Hello world, I'm back!! My name is arnie shawaszenegger"; //55 elements, 1 null terminator = 56

		// This is our String type we're going to serialize/deserialize
		const bit_packing_types::String<uint16_t> string(sampleString);

		// lets serialize to buffer
		string.Write(bitPacker);
		
		// lets deserialize to into another packet
		bit_packing_types::String<uint16_t> anotherString {};
		anotherString.Read(bitReader);

		EXPECT_STREQ(anotherString.c_str(), sampleString);
		EXPECT_EQ(string.NumBits(), 456);
	}


	TEST_F(BitPackingTests, PackString)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		// Need to write these
		const bit_packing_types::String<uint16_t> Name("Stuart Mathews");	
		constexpr int age38 = 38;
		constexpr int age39 = 39;
		constexpr int age40 = 40;

		// Need to read these

		bit_packing_types::String<uint16_t> tempName;
		
		// Write: //
			
		Name.Write(packer);
			EXPECT_EQ(packer.TotalBitsPacked(), 128);
			EXPECT_EQ(packer.BitsLeftInCurrentSegment(), 16);
		packer.Finish(); // write and start on a new segment
			EXPECT_EQ(packer.TotalBitsPacked(), 144);
			EXPECT_EQ(packer.BitsLeftInCurrentSegment(), 16);
		packer.Pack(BITS_REQUIRED(0,40), age38); // pack behind it
			EXPECT_EQ(packer.TotalBitsPacked(), 150);
			EXPECT_EQ(packer.BitsLeftInCurrentSegment(), 10);
		packer.Pack(BITS_REQUIRED(0,40), age39); // pack behind it
			EXPECT_EQ(packer.TotalBitsPacked(), 156);
			EXPECT_EQ(packer.BitsLeftInCurrentSegment(), 4);
		packer.Pack(BITS_REQUIRED(0,40), age40); // pack behind it
			EXPECT_EQ(packer.TotalBitsPacked(), 162);
			EXPECT_EQ(packer.BitsLeftInCurrentSegment(), 14);
		packer.Finish(); // flush any bits still in segment buffer to memory
			EXPECT_EQ(packer.TotalBitsPacked(), 176);

		
		// Read: //

		tempName.Read(reader);
			EXPECT_EQ(reader.TotalBitsRead(), 128);	
			EXPECT_EQ(reader.BitsLeftInSegment(), 16);
		reader.Finish(); // mimic the packer's finish after writing name
			EXPECT_EQ(reader.TotalBitsRead(), 144);
			EXPECT_EQ(reader.BitsLeftInSegment(), 16);
		EXPECT_EQ(reader.ReadNext<int>(BITS_REQUIRED(0,40)), age38);
			EXPECT_EQ(reader.TotalBitsRead(), 150);
			EXPECT_EQ(reader.BitsLeftInSegment(), 10);
		EXPECT_EQ(reader.ReadNext<int>(BITS_REQUIRED(0,40)), age39);
			EXPECT_EQ(reader.TotalBitsRead(), 156);
			EXPECT_EQ(reader.BitsLeftInSegment(), 4);
		EXPECT_EQ(reader.ReadNext<int>(BITS_REQUIRED(0,40)), age40);
			EXPECT_EQ(reader.TotalBitsRead(), 162);	
			EXPECT_EQ(reader.BitsLeftInSegment(), 14);
		reader.Finish();
			EXPECT_EQ(reader.TotalBitsRead(), 176);

		EXPECT_STREQ(Name.c_str(), tempName.c_str());
		
	}


	TEST_F(BitPackingTests, PackMixNumbersAndStrings)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		// Need to write these
		const bit_packing_types::String<uint16_t> Name("Stuart");	
		const bit_packing_types::String<uint16_t> NickName("Stu");
		const bit_packing_types::String<uint16_t> SurnameName("Mathews");
		constexpr int age38 = 38;
		constexpr int age39 = 39;
		constexpr int age40 = 40;

		// Need to read these

		bit_packing_types::String<uint16_t> tempName;
		bit_packing_types::String<uint16_t> tempNickName;
		bit_packing_types::String<uint16_t> tempSurnameName;
		
		// Write:
			
		Name.Write(packer); packer.Finish();
		packer.Pack(BITS_REQUIRED(0,40), age38); // pack behind it
		packer.Pack(BITS_REQUIRED(0,40), age39); // pack behind it
		packer.Pack(BITS_REQUIRED(0,40), age40); // pack behind it
		packer.Finish(); // flush any bits still in segment buffer to memory
		
		// Read:

		tempName.Read(reader); reader.Finish(); // mimic the packer's finish after writing name
		EXPECT_EQ(reader.ReadNext<int>(BITS_REQUIRED(0,40)), age38);
		EXPECT_EQ(reader.ReadNext<int>(BITS_REQUIRED(0,40)), age39);
		EXPECT_EQ(reader.ReadNext<int>(BITS_REQUIRED(0,40)), age40);
		reader.Finish();

		EXPECT_STREQ(Name.c_str(), tempName.c_str());

		// Write some more

		NickName.Write(packer);

		// Read some more
		tempNickName.Read(reader);
		
		EXPECT_STREQ(tempNickName.c_str(), NickName.c_str());

		// Write some more 
		SurnameName.Write(packer);

		// Read some more
		tempSurnameName.Read(reader);
			
		EXPECT_STREQ(tempSurnameName.c_str(), SurnameName.c_str());

	}

	TEST_F(BitPackingTests, PackMultipleMisAlignedStrings)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		// Need to write these
		const bit_packing_types::String<uint16_t> A("A"); // misaligned with 16bit buffer
		const bit_packing_types::String<uint16_t> AB("AB");
		const bit_packing_types::String<uint16_t> ABC("ABC");

		A.Write(packer); // should pack a 16bit value (1) and 8bits for the character A

		EXPECT_EQ(packer.TotalBitsPacked(), 24);
		EXPECT_EQ(packer.BitsLeftInCurrentSegment(), 8);

		bit_packing_types::String<uint16_t> readA;
		readA.Read(reader); // should read 16 bits for number 1, followed by 8 bits with 8 bits left over

		EXPECT_EQ(reader.TotalBitsRead(), 24); //test 
		EXPECT_EQ(reader.BitsLeftInSegment(), 8); // test
		EXPECT_STREQ(readA.c_str(), A.c_str());

		AB.Write(packer); // tries to pack 16 bits for the length 2 into remaining 8 bits, can't so packs half into it and writes the reamining 8 in the next
		// segment, so 24 + 8 + 8 = 40 with 8 left over. So when we try pack the 2 character string, it realizes its not at a buffer boundary and flushes it
		// so we are at 48bits before eriting the characters. So 48+16 = 64

		EXPECT_EQ(packer.TotalBitsPacked(), 64);
		EXPECT_EQ(packer.BitsLeftInCurrentSegment(), 16); // new segment
		

		bit_packing_types::String<uint16_t> readAB;
		readAB.Read(reader); // should reader 16 bits but find that 8 bits left in current segment, so it should

		EXPECT_EQ(reader.TotalBitsRead(), 64);
		EXPECT_EQ(reader.BitsLeftInSegment(), 16); // test
		EXPECT_STREQ(readAB.c_str(), AB.c_str());

		ABC.Write(packer);

		bit_packing_types::String<uint16_t> readABC;
		readABC.Read(reader);

		EXPECT_STREQ(readABC.c_str(), ABC.c_str());	
	}

	TEST_F(BitPackingTests, ManyPackLangthsOfStrings)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		// Write finals|semi|the|to|in|are|Eng
		// read after each write

		const bit_packing_types::String<uint16_t> england("England"); // 7 chars is 56 which is misaligned
		const bit_packing_types::String<uint16_t> are("are"); // 3 chars is 24 bits which is misaligned
		const bit_packing_types::String<uint16_t> in("in"); // 2 chars are 16 bits which is aligned
		const bit_packing_types::String<uint16_t> to("to"); // ''
		const bit_packing_types::String<uint16_t> the("the"); // see are
		const bit_packing_types::String<uint16_t> semi("semi"); // 4 chars are 32 which is aligned
		const bit_packing_types::String<uint16_t> finals("finals"); // 6 chars are 48 which is aligned

		england.Write(packer);

		bit_packing_types::String<uint16_t> tempEngland;
		tempEngland.Read(reader);
		
		are.Write(packer);

		bit_packing_types::String<uint16_t> tempAre;
		tempAre.Read(reader);

		in.Write(packer);

		bit_packing_types::String<uint16_t> tempIn;
		tempIn.Read(reader);

		to.Write(packer);

		bit_packing_types::String<uint16_t> tempTo;
		tempTo.Read(reader);

		the.Write(packer);

		bit_packing_types::String<uint16_t> tempThe;
		tempThe.Read(reader);

		semi.Write(packer);

		bit_packing_types::String<uint16_t> tempSemi;
		tempSemi.Read(reader);

		finals.Write(packer);

		bit_packing_types::String<uint16_t> tempFinals;
		tempFinals.Read(reader);
		
		EXPECT_STREQ(tempEngland.c_str(), england.c_str());
		EXPECT_STREQ(tempAre.c_str(), are.c_str());
		EXPECT_STREQ(tempIn.c_str(), in.c_str());
		EXPECT_STREQ(tempTo.c_str(), to.c_str());
		EXPECT_STREQ(tempThe.c_str(), the.c_str());
		EXPECT_STREQ(tempSemi.c_str(), semi.c_str());
		EXPECT_STREQ(tempFinals.c_str(), finals.c_str());


	}

	TEST_F(BitPackingTests, ManyPackLangthsOfStringsReadLate)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		// Write finals|semi|the|to|in|are|Eng
		// Wait to read once everything is written to reader's buffer

		const bit_packing_types::String<uint16_t> england("Eng"); // 7 chars is 56 which is misaligned
		const bit_packing_types::String<uint16_t> are("are"); // 3 chars is 24 bits which is misaligned
		const bit_packing_types::String<uint16_t> in("in"); // 2 chars are 16 bits which is aligned
		const bit_packing_types::String<uint16_t> to("to"); // ''
		const bit_packing_types::String<uint16_t> the("the"); // see are
		const bit_packing_types::String<uint16_t> semi("semi"); // 4 chars are 32 which is aligned
		const bit_packing_types::String<uint16_t> finals("finals"); // 6 chars are 48 which is aligned

		bit_packing_types::String<uint16_t> tempEngland;
		bit_packing_types::String<uint16_t> tempAre;	
		bit_packing_types::String<uint16_t> tempIn;
		bit_packing_types::String<uint16_t> tempTo;
		bit_packing_types::String<uint16_t> tempThe;
		bit_packing_types::String<uint16_t> tempSemi;
		bit_packing_types::String<uint16_t> tempFinals;

		england.Write(packer);
		are.Write(packer);
		in.Write(packer);
		to.Write(packer);
		the.Write(packer);
		semi.Write(packer);
		finals.Write(packer);

		tempEngland.Read(reader);	
		tempAre.Read(reader);	
		tempIn.Read(reader);
		tempTo.Read(reader);
		tempThe.Read(reader);
		tempSemi.Read(reader);
		tempFinals.Read(reader);
		
		EXPECT_STREQ(tempEngland.c_str(), england.c_str());
		EXPECT_STREQ(tempAre.c_str(), are.c_str());
		EXPECT_STREQ(tempIn.c_str(), in.c_str());
		EXPECT_STREQ(tempTo.c_str(), to.c_str());
		EXPECT_STREQ(tempThe.c_str(), the.c_str());
		EXPECT_STREQ(tempSemi.c_str(), semi.c_str());
		EXPECT_STREQ(tempFinals.c_str(), finals.c_str());
	}

	TEST_F(BitPackingTests, EmptyStrings)
	{
		uint16_t buffer[50];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		constexpr auto numberBits = 16;

		// write 1
		packer.Pack(numberBits, 1);

		// write Stu
		const bit_packing_types::String<uint16_t> Name("Stu");
		Name.Write(packer);

		// Write 2
		packer.Pack(numberBits, 2);

		// Write empty string
		const bit_packing_types::String<uint16_t> Surname("");
		Surname.Write(packer);

		// write Robert Charles
		const bit_packing_types::String<uint16_t> Middle("Robert Charles");
		Middle.Write(packer);

		// write 3
		packer.Pack(numberBits, 3);
		
		// read 1
		EXPECT_EQ(reader.ReadNext<int>(numberBits), 1);
		
		// read Stu
		bit_packing_types::String<uint16_t> readName;
		readName.Read(reader);

		// Read 2
		EXPECT_EQ(reader.ReadNext<int>(numberBits), 2);

		// Ready Empty string
		bit_packing_types::String<uint16_t> readSurname;
		readSurname.Read(reader);


		// Read middle name

		bit_packing_types::String<uint16_t> readMiddle;
		readMiddle.Read(reader);

		// read 3

		EXPECT_EQ(reader.ReadNext<int>(numberBits), 3);

		EXPECT_STREQ(readName.c_str(), Name.c_str());
		EXPECT_STREQ(readSurname.c_str(), Surname.c_str());
		EXPECT_STREQ(readMiddle.c_str(), Middle.c_str());


	}

	TEST_F(BitPackingTests, PackACustomFormat)
	{

		// Write: 4|Stu|1

		uint16_t buffer[50];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		constexpr auto numberBits = 16;

		packer.Pack(numberBits, 1);

		const bit_packing_types::String<uint16_t> Name("Stu");
		Name.Write(packer);

		packer.Pack(numberBits, 4);

		EXPECT_EQ(reader.ReadNext<int>(numberBits), 1);

		bit_packing_types::String<uint16_t> readName;
		readName.Read(reader);

		EXPECT_EQ(reader.ReadNext<int>(numberBits), 4);

		EXPECT_STREQ(readName.c_str(), Name.c_str());
	}

	TEST_F(BitPackingTests, ArrayOfStrings)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		bit_packing_types::ArrayOfStrings<uint16_t> arrayOfStrings;

		const std::string array[5] = { "Stuart"," Mathews", "was", "here", "!"}; 

		arrayOfStrings.Initialize(array, 5);

		arrayOfStrings.Write(packer);

		bit_packing_types::ArrayOfStrings<uint16_t> readArray;

		readArray.Read(reader);

		EXPECT_EQ(readArray.NumElements(), arrayOfStrings.NumElements());

		EXPECT_STREQ(readArray[0].c_str(), arrayOfStrings[0].c_str());
		EXPECT_STREQ(readArray[1].c_str(), arrayOfStrings[1].c_str());
		EXPECT_STREQ(readArray[2].c_str(), arrayOfStrings[2].c_str());
		EXPECT_STREQ(readArray[3].c_str(), arrayOfStrings[3].c_str());
		EXPECT_STREQ(readArray[4].c_str(), arrayOfStrings[4].c_str());
	}

	TEST_F(BitPackingTests, ByteArray)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		constexpr int size = 6;

		char array[size] = { 'a', 'b', 'c', 'd', 'e', 'f'};

		const bit_packing_types::ByteArray<uint16_t> byteArray(array, size);

		EXPECT_EQ(byteArray.Size(), size);

		EXPECT_EQ(byteArray.data()[0], 'a');
		EXPECT_EQ(byteArray.data()[1], 'b');
		EXPECT_EQ(byteArray.data()[2], 'c');
		EXPECT_EQ(byteArray.data()[3], 'd');
		EXPECT_EQ(byteArray.data()[4], 'e');
		EXPECT_EQ(byteArray.data()[5], 'f');

		byteArray.Write(packer);

		bit_packing_types::ByteArray<uint16_t> tempArray;
		tempArray.Read(reader);

		EXPECT_EQ(tempArray.data()[0], 'a');
		EXPECT_EQ(tempArray.data()[1], 'b');
		EXPECT_EQ(tempArray.data()[2], 'c');
		EXPECT_EQ(tempArray.data()[3], 'd');
		EXPECT_EQ(tempArray.data()[4], 'e');
		EXPECT_EQ(tempArray.data()[5], 'f');

		EXPECT_EQ(tempArray.Size(), size);

	}

	TEST_F(BitPackingTests, ArrayOfByteArray)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		constexpr int size1 = 6;
		constexpr int size2 = 7;
		constexpr int size3 = 8;

		constexpr char array1[size1] = { 'a', 'b', 'c', 'd', 'e', 'f'};
		constexpr char array2[size2] = { 'g', 'h', 'i', 'j', 'k', 'l', 'm'};
		constexpr char array3[size3] = { 'n', 'o', 'p', 'q', 'r', 's', 't', 'u'};

		// 3 byte arrays
		bit_packing_types::ByteArray<uint16_t> byteArray[3];
		byteArray[0] = bit_packing_types::ByteArray<uint16_t>(array1, size1);
		byteArray[1] = bit_packing_types::ByteArray<uint16_t>(array2, size1);
		byteArray[2] = bit_packing_types::ByteArray<uint16_t>(array3, size1);

		// and array of byte arrays
		bit_packing_types::ArrayOfBytes<uint16_t> arrayOfBytes;

		// init
		arrayOfBytes.Initialize(byteArray, 3);

		EXPECT_EQ(arrayOfBytes.NumElements(), 3);

		// write to buffer
		arrayOfBytes.Write(packer);

		// read from buffer
		bit_packing_types::ArrayOfBytes<uint16_t> tempArrayOfBytes;
		tempArrayOfBytes.Read(reader);

		// still everything looks ok?

		for(int i =0; i < tempArrayOfBytes[0].Size();i++)
		{
			EXPECT_EQ(tempArrayOfBytes[0].data()[i], array1[i]);
		}
		for(int i =0; i < tempArrayOfBytes[1].Size();i++)
		{
			EXPECT_EQ(tempArrayOfBytes[1].data()[i], array2[i]);
		}
		for(int i =0; i < tempArrayOfBytes[2].Size();i++)
		{
			EXPECT_EQ(tempArrayOfBytes[2].data()[i], array3[i]);
		}
		
	}

	TEST_F(BitPackingTests, PacketDatumSerilization)
	{
		uint16_t buffer[32];
		
		BitPacker packer(buffer, 32);
		BitfieldReader reader(buffer, 32);

		auto data1 = "data1";
		auto data2 = "data2";
		auto data3 = "data3";

		PacketDatum d1(false, data1);
		PacketDatum d2(false, data2);
		PacketDatum d3(false, data3);

		d1.Write(packer);
		d2.Write(packer);
		d3.Write(packer);

		PacketDatum tempD1;	
		PacketDatum tempD2;	
		PacketDatum tempD3;

		tempD1.Read(reader);
		tempD2.Read(reader);
		tempD3.Read(reader);

		EXPECT_EQ(tempD1.Sequence, d1.Sequence);
		EXPECT_EQ(tempD1.IsAcked, d1.IsAcked);
		EXPECT_STREQ(tempD1.Data(), d1.Data());	

		EXPECT_EQ(tempD2.Sequence, d2.Sequence);
		EXPECT_EQ(tempD2.IsAcked, d2.IsAcked);
		EXPECT_STREQ(tempD2.Data(), d2.Data());	

		EXPECT_EQ(tempD3.Sequence, d3.Sequence);
		EXPECT_EQ(tempD3.IsAcked, d3.IsAcked);
		EXPECT_STREQ(tempD3.Data(), d3.Data());
	}
}