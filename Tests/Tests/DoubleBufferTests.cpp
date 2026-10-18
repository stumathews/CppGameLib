#include "pch.h"
#include <gtest/gtest.h>
#include "buffering/DoubleBuffer.h"

namespace gamelib
{
	class DoubleBufferTests : public testing::Test
	{
	public:

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};

	TEST_F(DoubleBufferTests, SimpleUsage)
	{
		DoubleBuffer<int> taskData;

		// Simulate a background thread writing to the taskData buffer
		taskData.GetWriteBuffer() = 42;

		// Simulate the background thread publishing the written data
		taskData.Publish();

		// Simulate the main thread reading the written data
		auto data = taskData.GetReadBuffer();

		EXPECT_EQ(*data, 42);

		// Read it again to make sure it doesn't change until published again
		data = taskData.GetReadBuffer();
		EXPECT_EQ(*data, 42);

		// Write a new value to the back buffer
		taskData.GetWriteBuffer() = 43;

		// Back sure the front buffer still has the old value
		data = taskData.GetReadBuffer();
		EXPECT_EQ(*data, 42);

		// Publish the new value
		taskData.Publish();

		// Now the front buffer should have the new value
		data = taskData.GetReadBuffer();
		EXPECT_EQ(*data, 43);

	}
};