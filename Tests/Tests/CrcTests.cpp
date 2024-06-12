
#include "pch.h"
#include "net/crc.h"

using namespace gamelib;

class CrcTests : public testing::Test
{
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
	const auto crc = crc32buf(buffer, strlen(buffer));

	const auto buffer1 = "Dude where is my car?";
	const auto crc1 = crc32buf(buffer1, strlen(buffer1));

	EXPECT_EQ(crc, 0x435ef19c);
	EXPECT_NE(crc1, 0x435ef19c);
	EXPECT_EQ(crc1, 0x767f565e);

}
