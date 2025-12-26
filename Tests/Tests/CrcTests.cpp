
#include "pch.h"

#include <gtest/gtest.h>

#include "net/crc.h"
#include "net/Message.h"
#include "net/PacketDatum.h"

namespace gamelib
{
	class CrcTests : public testing::Test {
	public:	

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};

	TEST_F(CrcTests, CRC32)
	{
		const auto buffer = "There can be only one.";
		const Crc32 crc32;
		const auto crc = crc32.compute((uint8_t*)buffer, strlen(buffer));

		const auto buffer1 = "Dude where is my car?";
		const auto crc1 = crc32.compute((uint8_t*)buffer1, strlen(buffer1));

		EXPECT_EQ(crc, 0x435ef19c);
		EXPECT_NE(crc1, 0x435ef19c);
		EXPECT_EQ(crc1, 0x767f565e);

	}


	TEST_F(CrcTests, CheckSumTest)
	{
		const auto seq1 = 6;
		const auto lastAckedSeq = 2;
		const auto previousAckedBits = 1;
		std::vector<PacketDatum> data;

		const auto datum1 = PacketDatum(false, "data for datum1");
		const auto datum2 = PacketDatum(false, "data for datum2");

		data.push_back(datum1);
		data.push_back(datum2);

		Message msg1(seq1, lastAckedSeq, previousAckedBits, data.size(), data, General);

		const auto checksum1 = msg1.GenerateCheckSum();

		auto msg2 = msg1;

		EXPECT_TRUE(msg2.ValidateChecksum(checksum1));
		EXPECT_TRUE(msg2.ValidateChecksum());

		auto msg3 = msg1;
		msg3.Header.LastAckedSequence = 4;

		EXPECT_FALSE(msg3.ValidateChecksum(checksum1));

	}
}
