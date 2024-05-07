
#include "pch.h"
#include <net/Networking.h>
#include "net/NetworkManager.h"
#include "events/EventManager.h"
#include "utils/RingBuffer.h"
#include <stdbool.h>

#include "utils/BitFiddler.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace gamelib;

struct PacketDatum
{
	PacketDatum(){ Acked = false; }
	explicit PacketDatum(const bool acked, const char* customData = "") : Acked(acked), CustomData(customData) {  }

    bool Acked;
	uint16_t Sequence {};
	double SendTime {};
	const char* CustomData;

};

struct MessageHeader
{
    uint16_t Sequence;
    uint16_t LastAckedSequence;
    uint32_t LastAckedBits; // bit n is set means Sequence - n was acked
};

class Message
{
public:

	explicit Message(const uint16_t sequence, const uint16_t lastAckedSequence, uint32_t previousAckedBits,
	                 const uint16_t n = 0, const PacketDatum* inData = nullptr)
	{
		// Set the sequence for this message in the header
		Header.Sequence = sequence;
		Header.LastAckedSequence = lastAckedSequence;
		Header.LastAckedBits = previousAckedBits;
		
		for(auto i = 0; i < n; i++)
		{
			data.push_back(inData[i]);
		}
	}

	MessageHeader Header;
	[[nodiscard]] std::vector<PacketDatum> Data() const { return data; }
	[[nodiscard]] uint16_t DataCount() const { return data.size(); }
private:
	std::vector<PacketDatum> data;
};

class ReliableUdp
{
public:	

	Message* Send(PacketDatum datum)
	{
		sequence++;
		std::vector<PacketDatum> data = {};

		// add data to message
		datum.Sequence = sequence; // stamp the packet with indication of the sequence we will use
		data.push_back(datum);

		// add to send buffer
		sendBuffer.Put(sequence, datum);

		// Prepare a message to send. It may contain other data that was not acked previously

		// we'll send any unacked data that is supposed to have been sent
		for(uint16_t i = 1; i <= sendBuffer.GetBufferSize(); i++)
		{
			auto pCurrentPacket = sendBuffer.Get(sequence - i);

			if(pCurrentPacket == nullptr) continue;

			auto currentPacket = *pCurrentPacket;
			
			if(!currentPacket.Acked)
			{
				data.push_back(currentPacket);
			}
		}

		// Send Message
		sentMessage = new Message(sequence, lastAckededSequence, GenerateAckedBits(), data.size(), data.data());
		return sentMessage;
	}

	void Receive(const Message& message)
	{
		const auto messageSequence = message.Header.Sequence;
		
		// Mark the sender's acked 
		sendBuffer.Put(message.Header.LastAckedSequence, PacketDatum(true));
			
		// The sender also may have acked the previous 32 packets, also remove those
		for(uint16_t i = 1; i <= sizeof(message.Header.LastAckedBits) * 8 ;i++)
		{
			if(BitFiddler<uint32_t>::BitCheck(message.Header.LastAckedBits, i))
			{
				const uint16_t currentSequence = message.Header.LastAckedSequence - i;
				sendBuffer.Put(currentSequence, PacketDatum(true));
			}
		}
			
		// mark incoming sequence as received
		PacketDatum incomingData = message.Data()[0];
		incomingData.Acked = true;
		receiveBuffer.Put(messageSequence, incomingData);

		// mark all the containing sequences as having been received too
		for(auto i = 1; i < message.Data().size()-1; i++)
		{
			const uint16_t currentSequence = messageSequence - i;
			auto thisData = message.Data()[currentSequence];
			thisData.Acked = true;
			receiveBuffer.Put(currentSequence, thisData);
		}
		
		if(messageSequence > lastAckededSequence)
		{
			lastAckededSequence = messageSequence;
		}
	}

	uint32_t GenerateAckedBits()
	{
		uint32_t previousAcked {};

		// get data for the last 32 acked packet sequences not including lastAckedSequence
		for(uint16_t i = 1; i < 32;i++)
		{
			const PacketDatum* pCurrentPacket = receiveBuffer.Get(lastAckededSequence - i);

			if(pCurrentPacket != nullptr)
			{
				const PacketDatum currentPacket = *pCurrentPacket;
				if(currentPacket.Acked)
				{				
					previousAcked = BitFiddler<uint32_t>::SetBit(previousAcked, i);
					continue;
				}
			}

			previousAcked = BitFiddler<uint32_t>::ClearBit(previousAcked, i);		
			
		}

		return previousAcked;
	}

	uint16_t sequence {};
	uint16_t lastAckededSequence {};

	RingBuffer<PacketDatum> receiveBuffer;
	RingBuffer<PacketDatum> sendBuffer;
	Message* sentMessage;
private:
	
};

class ReliableUdpTests : public testing::Test
{
public:	

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

template <class T>
uint32_t GenerateAckBits(RingBuffer<T> buffer)
{
	
}


TEST_F(ReliableUdpTests, BasicSend)
{
	ReliableUdp reliableUdp;

	const auto data1 = PacketDatum( false, "data1");
	const auto data2 = PacketDatum( false, "data2");
	const auto data3 = PacketDatum( false, "data3");
		
	// Send

	const auto* message1 = reliableUdp.Send(data1);	
	EXPECT_EQ(reliableUdp.sendBuffer.GetLastAddedSequence(), message1->Header.Sequence);

	const auto* message2 = reliableUdp.Send(data2);	
	EXPECT_EQ(reliableUdp.sendBuffer.GetLastAddedSequence(), message2->Header.Sequence);

	const auto* message3 = reliableUdp.Send(data3);	
	EXPECT_EQ(reliableUdp.sendBuffer.GetLastAddedSequence(), message3->Header.Sequence);
	
	EXPECT_TRUE(message1->DataCount(), 1);
	EXPECT_EQ(message1->Header.Sequence, 1);
	EXPECT_STREQ(message1->Data()[0].CustomData, data1.CustomData);
	EXPECT_EQ(message1->Header.LastAckedBits, 0);
	EXPECT_EQ(message1->Header.LastAckedSequence, 0);
	
	EXPECT_TRUE(message2->DataCount(), 2);
	EXPECT_EQ(message2->Header.Sequence, 2);
	EXPECT_EQ(message2->Header.LastAckedBits, 0);
	EXPECT_EQ(message2->Header.LastAckedSequence, 0);

	// Should also contain message 1 as it it not acked
	EXPECT_STREQ(message2->Data()[0].CustomData, data2.CustomData);
	EXPECT_STREQ(message2->Data()[1].CustomData, data1.CustomData);

	EXPECT_TRUE(message3->DataCount(), 3);
	EXPECT_EQ(message3->Header.Sequence, 3);
	EXPECT_EQ(message3->Header.LastAckedBits, 0);
	EXPECT_EQ(message3->Header.LastAckedSequence, 0);

	// should also contain message1, message2 as they are also not acked
	EXPECT_STREQ(message3->Data()[0].CustomData, data3.CustomData);
	EXPECT_STREQ(message3->Data()[1].CustomData, data2.CustomData);
	EXPECT_STREQ(message3->Data()[2].CustomData, data1.CustomData);

}

TEST_F(ReliableUdpTests, BasicRecieve)
{
	ReliableUdp reliableUdp;

	const auto data1 = PacketDatum( false, "data1");
	const auto data2 = PacketDatum( false, "data2");
	const auto data3 = PacketDatum( false, "data3");

	const auto* message1 = reliableUdp.Send(data1);
	reliableUdp.Receive(*message1);

	EXPECT_EQ(reliableUdp.receiveBuffer.GetLastAddedSequence(), message1->Header.Sequence);
	EXPECT_EQ(reliableUdp.lastAckededSequence, message1->Header.Sequence);
	EXPECT_TRUE(reliableUdp.receiveBuffer.Get(message1->Header.Sequence)->Acked);

	const auto* message2 = reliableUdp.Send(data2);
	reliableUdp.Receive(*message2);

	EXPECT_EQ(reliableUdp.receiveBuffer.GetLastAddedSequence(), message2->Header.Sequence);
	EXPECT_EQ(reliableUdp.lastAckededSequence, message2->Header.Sequence);
	EXPECT_TRUE(reliableUdp.receiveBuffer.Get(message2->Header.Sequence)->Acked);

	const auto* message3 = reliableUdp.Send(data3);
	reliableUdp.Receive(*message3);

	EXPECT_EQ(reliableUdp.receiveBuffer.GetLastAddedSequence(), message3->Header.Sequence);
	EXPECT_EQ(reliableUdp.lastAckededSequence, message3->Header.Sequence);
	EXPECT_TRUE(reliableUdp.receiveBuffer.Get(message3->Header.Sequence)->Acked);

	auto senderAckedBits = reliableUdp.GenerateAckedBits();

	EXPECT_FALSE(BitFiddler<uint32_t>::BitCheck(senderAckedBits, 0)); // if bit n is set, means the nth prior sequence was received eg. 3-0 = sequence 3
	EXPECT_TRUE(BitFiddler<uint32_t>::BitCheck(senderAckedBits, 1));  // if bit n is set, means the nth prior sequence was received eg. 3-1 = sequence 2
	EXPECT_TRUE(BitFiddler<uint32_t>::BitCheck(senderAckedBits, 2));  // if bit n is set, means the nth prior sequence was received eg. 3-2 = sequence 1
}

TEST_F(ReliableUdpTests, AliceBobBasic)
{
	ReliableUdp alice;
	ReliableUdp bob;

	// alice -[a1]-> bob:

	const auto a1 = PacketDatum( false, "a1");

	// Send a1 to bob along with any indications that alice has received previously sent from bob,
	// which would be none as bob as not sent alice anything yet
	const auto a1SentMessage = *alice.Send(a1);

	// bob receives it and marks that it was received. The next message sent to alice will inform alice that it was received
	bob.Receive(a1SentMessage);

	// As nothing from bob has been sent to alice to inform alice that bob received anything from alice, alice should still consider what it sent
	// as having not been acknowledged
	EXPECT_FALSE(alice.sendBuffer.Get(a1SentMessage.Header.Sequence)->Acked);

	// bob -[a1ack]-> alice:

	const auto b1 = PacketDatum( false, "b1");

	// Bob will send something to alice, and say that it knows about the last thing alice sent him, which if it happens to be what alice sent him
	// which it it, alice can mark off that it was acked
	const auto sentA1AckMessage = *bob.Send(b1);

	alice.Receive(sentA1AckMessage);

	// Expect that that bob's mesage header contained the last ack sequence, which alice removed from alice's send buffer,
	EXPECT_TRUE(alice.sendBuffer.Get(a1SentMessage.Header.Sequence)->Acked);

	// Ensure alice does not send it again
	const auto a2 = PacketDatum(false, "a2");
	const auto a2SentMessage = alice.Send(a2);
	EXPECT_TRUE(a2SentMessage->DataCount(), 1);
	EXPECT_EQ(a2SentMessage->Data()[0].Sequence, a2SentMessage->Header.Sequence);
}

TEST_F(ReliableUdpTests, AliceBobAggregateMessages)
{
	const auto a1 = PacketDatum(false, "a1");
	const auto a2 = PacketDatum(false, "a2");
	const auto a3 = PacketDatum(false, "a3");

	ReliableUdp alice;
	ReliableUdp bob;

	const auto a1SentMessage = *alice.Send(a1);
	const auto a2SentMessage = *alice.Send(a2);
	const auto a3SentMessage = *alice.Send(a3);

	bob.Receive(a1SentMessage);
	bob.Receive(a2SentMessage);
	bob.Receive(a3SentMessage);

	const auto b1 = PacketDatum(false, "b1");

	const auto b1SentMessage = *bob.Send(b1);
	const auto bobReceivedAcks = BitFiddler<uint32_t>::ToString(b1SentMessage.Header.LastAckedBits);
	alice.Receive(b1SentMessage);

	// Alice should know know that only a1 and a3 were received

	EXPECT_TRUE(alice.sendBuffer.Get(a1SentMessage.Header.Sequence)->Acked);
	EXPECT_FALSE(alice.sendBuffer.Get(a2SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a3SentMessage.Header.Sequence)->Acked);
	
}