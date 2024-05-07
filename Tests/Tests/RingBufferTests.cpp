
#include <gtest/gtest.h>

#include "utils/RingBuffer.h"

using namespace gamelib;

class RingBufferTests : public testing::Test
{
public:	

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(RingBufferTests, RingBufferBasic)
{
	constexpr auto data1 = 1;
	constexpr auto data2 = 2;
	constexpr auto data3 = 3;
	constexpr auto data4 = 4;
	constexpr auto bufferSize = 2;

	RingBuffer<int> buffer(bufferSize);

	// add data to buffer

	buffer.Put(99, data1);
	buffer.Put(100, data2);

	const auto result1 = *buffer.Get(99);
	const auto result2 = *buffer.Get(100);

	// Overwrite buffer with new data
	buffer.Put(101, data3);
	buffer.Put(102, data4);
	
	const auto result3 = *buffer.Get(101);
	const auto result4 = *buffer.Get(102);


	// Try and use old position indexes
	const auto result5 = buffer.Get(99);
	const auto result6 = buffer.Get(100);
	
	// try get data for previously unset index

	const auto result7 = buffer.Get(456);
	
	EXPECT_EQ(buffer.GetBufferSize(), bufferSize);

	EXPECT_EQ(result1, data1);
	EXPECT_EQ(result2, data2);
	EXPECT_EQ(result3, data3);
	EXPECT_EQ(result4, data4);

	// should only correspond to the new data as that has overwritten the old data
	EXPECT_EQ(result5, nullptr);
	EXPECT_EQ(result6, nullptr);

	// Unknown index should return {}
	EXPECT_EQ(result7, nullptr);
}
