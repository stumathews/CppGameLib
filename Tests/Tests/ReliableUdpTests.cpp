
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
		// increase sequence number for this datum to be sent
		datum.Sequence = ++sequence;

		// Prepare the package of data that will be sent starting with the datum that's intended to be sent
		std::vector dataToSent = { datum };		

		// add to send buffer
		sendBuffer.Put(sequence, datum);
		
		// We'll attach any unacked data that was supposed to have been sent previously
		for(uint16_t i = 1; i <= sendBuffer.GetBufferSize(); i++)
		{
			// Get previous sequence and see if its unacked...
			const auto pPreviousDatum = sendBuffer.Get(sequence - i);

			if(pPreviousDatum == nullptr) continue;

			auto previousDatum = *pPreviousDatum;

			// We have some previous data that was not sent.
			if(!previousDatum.Acked)
			{
				// Add it to what must be sent with this message
				dataToSent.push_back(previousDatum);
			}
		}

		// Send Message, attaching any consecutive data that was not previously sent
		// also include a reference to what we've received from the sender previously, just in case an ack did not go through to the sender
		sentMessage = new Message(sequence, lastAckededSequence, GeneratePreviousAckedBits(), dataToSent.size(), dataToSent.data());
		return sentMessage;
	}

	void Receive(const Message& message)
	{
		const auto messageSequence = message.Header.Sequence;
		
		// Mark the sender's last known message it acked from us as acked, i.e no need to resend 
		sendBuffer.Put(message.Header.LastAckedSequence, PacketDatum(true));
			
		// Look through the sender's list of previously acked messages and ensure that we dont re-send them
		constexpr uint16_t numBits = sizeof message.Header.LastAckedBits * 8;
		for(uint16_t i = 1; i < numBits ;i++)
		{
			if(BitFiddler<uint32_t>::BitCheck(message.Header.LastAckedBits, i))
			{
				const uint16_t previousSequence = (message.Header.LastAckedSequence - i) + 1;
				auto datumToUpdate = *sendBuffer.Get(previousSequence); datumToUpdate.Acked = true;
				sendBuffer.Put(previousSequence, datumToUpdate);
			}
		}

		// mark all the containing prior sequences as having been received too
		for(uint16_t i = 1; i < message.Data().size()-1; i++)
		{
			const uint16_t currentSequence = messageSequence - i;
			auto datumToUpdate = message.Data()[currentSequence]; datumToUpdate.Acked = true;
			receiveBuffer.Put(currentSequence, datumToUpdate);
		}		
			
		// mark the this incoming sequence as finally received after all previous ones contained in the message
		PacketDatum incomingData = message.Data()[0]; incomingData.Acked = true;
		receiveBuffer.Put(messageSequence, incomingData);

		// last mark this as the last acknowledged sequence if we've not seen it before, i.e it larger than waht we've previously seen
		if(messageSequence > lastAckededSequence)
		{
			lastAckededSequence = messageSequence;
		}
	}

	uint32_t GeneratePreviousAckedBits()
	{
		uint32_t previousAckedBits {};

		// Bit 0 is always unset as it represents the current datm, while bit n represents (current - n) ie nth previous datum 
		previousAckedBits = BitFiddler<uint32_t>::ClearBit(previousAckedBits, 0);

		// Read the list of already received & acked data.
		for(uint32_t i = 0; i < sizeof previousAckedBits * 8; i++)
		{
			const PacketDatum* pCurrentPacket = receiveBuffer.Get(lastAckededSequence - i);

			if(pCurrentPacket != nullptr)
			{
				const PacketDatum currentPacket = *pCurrentPacket;
				if(currentPacket.Acked)
				{				
					previousAckedBits = BitFiddler<uint32_t>::SetBit(previousAckedBits, i + 1);
					continue;
				}
			}

			previousAckedBits = BitFiddler<uint32_t>::ClearBit(previousAckedBits, i + 1);					
		}
		
		// NB: we'll tell the receiver that this is what we've already received
		return previousAckedBits;
	}
	
	RingBuffer<PacketDatum> receiveBuffer;
	RingBuffer<PacketDatum> sendBuffer;	
	uint16_t lastAckededSequence {};
private:
	Message* sentMessage{};
	uint16_t sequence {};	
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

	auto senderAckedBits = reliableUdp.GeneratePreviousAckedBits();

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

	// alice -[a1]-> bob
	const auto a1SentMessage = *alice.Send(a1); bob.Receive(a1SentMessage);
	EXPECT_TRUE(bob.receiveBuffer.Get(a1SentMessage.Header.Sequence)->Acked);

	// alice -[a2]-> bob
	const auto a2SentMessage = *alice.Send(a2); //bob.Receive(a2SentMessage); // simulate bob not receiving it
	EXPECT_TRUE(bob.receiveBuffer.Get(a2SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it

	// alice -[a3]-> bob
	const auto a3SentMessage = *alice.Send(a3); bob.Receive(a3SentMessage);
	EXPECT_TRUE(bob.receiveBuffer.Get(a2SentMessage.Header.Sequence)->Acked); // this time we go it, because it was sent as part of message3 (it was sent again)
	EXPECT_TRUE(bob.receiveBuffer.Get(a3SentMessage.Header.Sequence)->Acked);

	// bob -[b1]-> alice
	const auto b1 = PacketDatum(false, "b1");
	const auto b1SentMessage = *bob.Send(b1); // Bob wil tell alice what it has received so far
	alice.Receive(b1SentMessage);

	// Bob's message should now be able to inform Alice that he received a1 a2 a3 were received

	EXPECT_TRUE(alice.sendBuffer.Get(a1SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a2SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a3SentMessage.Header.Sequence)->Acked);
	
}