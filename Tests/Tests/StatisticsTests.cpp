#include "pch.h"
#include <limits>
#include <gtest/gtest.h>

#include "net/PacketDatum.h"
#include "net/PacketDatumUtils.h"
#include "utils/RingBuffer.h"
#include "utils/Statistics.h"

using namespace gamelib;

class StatisticsTests : public testing::Test
{
public:	

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};


TEST_F(StatisticsTests, SMA)
{
	// consider the average over the last k entries of the dataset
	const auto k = 3;
	const std::vector<unsigned long> data1 = { 1, 2, 3, 4, 5, 6}; // n = 6
	const std::vector<unsigned long> data2 = { 1, 2, 3, 4, 5, 6, 7}; // n =7
	const std::vector<unsigned long> data3 = { 1, 2, 3, 4, 5, 6, 7, 8}; // n=8

	EXPECT_EQ(Statistics::SMA(k, data1), 5);
	EXPECT_EQ(Statistics::SMA(k, data2), 6);
	EXPECT_EQ(Statistics::SMA(k, data3), 7);

}

TEST_F(StatisticsTests, Latencies)
{
	RingBuffer<PacketDatum> receiveBuffer(6);

	auto d1 = PacketDatum(true);	
	auto d2 = PacketDatum(true);	
	auto d3 = PacketDatum(true);	
	auto d4 = PacketDatum(true);	
	auto d5 = PacketDatum(true);	
	auto d6 = PacketDatum(true);	

	d1.RttMs = 1; 
	d2.RttMs = 2;
	d3.RttMs = 3;
	d4.RttMs = 4;
	d5.RttMs = 5;
	d6.RttMs = 6;

	// Simulate a receive buffer of acked packet latencies

	receiveBuffer.Put(1, d1);
	receiveBuffer.Put(2, d2);
	receiveBuffer.Put(3, d3);
	receiveBuffer.Put(4, d4);
	receiveBuffer.Put(5, d5);
	receiveBuffer.Put(6, d6);

	constexpr auto lastKEntries = 3;

	// Get the data points as a series of latencies or rtts (round trip times)
	auto data = PacketDatumUtils::GetLastKRtts(6, receiveBuffer);

	constexpr auto expectedSma = 5;
	EXPECT_EQ(Statistics::SMA(lastKEntries, data), expectedSma);
}