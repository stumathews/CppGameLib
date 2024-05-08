
#include "pch.h"
#include <net/Networking.h>
#include "net/NetworkManager.h"
#include "events/EventManager.h"
#include "utils/RingBuffer.h"
#include "net/ReliableUdp.h"
#include "utils/BitFiddler.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace gamelib;

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


TEST_F(ReliableUdpTests, BasicSend)
{
	ReliableUdp reliableUdp;

	const auto data1 = ReliableUdp::PacketDatum( false, "data1");
	const auto data2 = ReliableUdp::PacketDatum( false, "data2");
	const auto data3 = ReliableUdp::PacketDatum( false, "data3");
		
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

	const auto data1 = ReliableUdp::PacketDatum( false, "data1");
	const auto data2 = ReliableUdp::PacketDatum( false, "data2");
	const auto data3 = ReliableUdp::PacketDatum( false, "data3");

	const auto* message1 = reliableUdp.Send(data1);
	reliableUdp.Receive(*message1);

	EXPECT_EQ(reliableUdp.receiveBuffer.GetLastAddedSequence(), message1->Header.Sequence);
	EXPECT_EQ(reliableUdp.lastAckedSequence, message1->Header.Sequence);
	EXPECT_TRUE(reliableUdp.receiveBuffer.Get(message1->Header.Sequence)->Acked);

	const auto* message2 = reliableUdp.Send(data2);
	reliableUdp.Receive(*message2);

	EXPECT_EQ(reliableUdp.receiveBuffer.GetLastAddedSequence(), message2->Header.Sequence);
	EXPECT_EQ(reliableUdp.lastAckedSequence, message2->Header.Sequence);
	EXPECT_TRUE(reliableUdp.receiveBuffer.Get(message2->Header.Sequence)->Acked);

	const auto* message3 = reliableUdp.Send(data3);
	reliableUdp.Receive(*message3);

	EXPECT_EQ(reliableUdp.receiveBuffer.GetLastAddedSequence(), message3->Header.Sequence);
	EXPECT_EQ(reliableUdp.lastAckedSequence, message3->Header.Sequence);
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

	const auto a1 = ReliableUdp::PacketDatum( false, "a1");

	// Send a1 to bob along with any indications that alice has received previously sent from bob,
	// which would be none as bob as not sent alice anything yet
	const auto a1SentMessage = *alice.Send(a1);

	// bob receives it and marks that it was received. The next message sent to alice will inform alice that it was received
	bob.Receive(a1SentMessage);

	// As nothing from bob has been sent to alice to inform alice that bob received anything from alice, alice should still consider what it sent
	// as having not been acknowledged
	EXPECT_FALSE(alice.sendBuffer.Get(a1SentMessage.Header.Sequence)->Acked);

	// bob -[a1ack]-> alice:

	const auto b1 = ReliableUdp::PacketDatum( false, "b1");

	// Bob will send something to alice, and say that it knows about the last thing alice sent him, which if it happens to be what alice sent him
	// which it it, alice can mark off that it was acked
	const auto sentA1AckMessage = *bob.Send(b1);

	alice.Receive(sentA1AckMessage);

	// Expect that that bob's mesage header contained the last ack sequence, which alice removed from alice's send buffer,
	EXPECT_TRUE(alice.sendBuffer.Get(a1SentMessage.Header.Sequence)->Acked);

	// Ensure alice does not send it again
	const auto a2 = ReliableUdp::PacketDatum(false, "a2");
	const auto a2SentMessage = alice.Send(a2);
	EXPECT_TRUE(a2SentMessage->DataCount(), 1);
	EXPECT_EQ(a2SentMessage->Data()[0].Sequence, a2SentMessage->Header.Sequence);
}

TEST_F(ReliableUdpTests, AliceBobAggregateMessagesDrop1)
{
	const auto a1 = ReliableUdp::PacketDatum(false, "a1");
	const auto a2 = ReliableUdp::PacketDatum(false, "a2");
	const auto a3 = ReliableUdp::PacketDatum(false, "a3");

	ReliableUdp alice;
	ReliableUdp bob;

	// alice -[a1]-> bob
	const auto a1SentMessage = *alice.Send(a1); bob.Receive(a1SentMessage);
	EXPECT_TRUE(bob.receiveBuffer.Get(a1SentMessage.Header.Sequence)->Acked);

	// alice -[a2]->X bob
	const auto a2SentMessage = *alice.Send(a2); //bob.Receive(a2SentMessage); // simulate bob not receiving it
	EXPECT_TRUE(bob.receiveBuffer.Get(a2SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it

	// alice -[a3]-> bob
	const auto a3SentMessage = *alice.Send(a3); bob.Receive(a3SentMessage);
	EXPECT_TRUE(bob.receiveBuffer.Get(a2SentMessage.Header.Sequence)->Acked); // this time we got it, because it was sent as part of message3 (it was sent again)
	EXPECT_TRUE(bob.receiveBuffer.Get(a3SentMessage.Header.Sequence)->Acked);

	// bob -[b1]-> alice
	const auto b1 = ReliableUdp::PacketDatum(false, "b1");
	const auto b1SentMessage = *bob.Send(b1); // Bob wil tell alice what it has received so far
	alice.Receive(b1SentMessage);

	// Bob's message should now be able to inform Alice that he received a1 a2 a3 were received

	EXPECT_TRUE(alice.sendBuffer.Get(a1SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a2SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a3SentMessage.Header.Sequence)->Acked);
	
}

TEST_F(ReliableUdpTests, AliceBobAggregateMessagesDrop3)
{
	const auto a1 = ReliableUdp::PacketDatum(false, "a1");
	const auto a2 = ReliableUdp::PacketDatum(false, "a2");
	const auto a3 = ReliableUdp::PacketDatum(false, "a3");
	const auto a4 = ReliableUdp::PacketDatum(false, "a4");
	const auto a5 = ReliableUdp::PacketDatum(false, "a5");

	ReliableUdp alice;
	ReliableUdp bob;

	// alice -[a1]-> bob
	const auto a1SentMessage = *alice.Send(a1); bob.Receive(a1SentMessage);

	// alice -[a2]->X bob
	const auto a2SentMessage = *alice.Send(a2); //bob.Receive(a2SentMessage); // simulate bob not receiving it
	EXPECT_TRUE(bob.receiveBuffer.Get(a2SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it

	// alice -[a3]->X bob
	const auto a3SentMessage = *alice.Send(a3); //bob.Receive(a3SentMessage);	
	EXPECT_TRUE(bob.receiveBuffer.Get(a3SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it
	EXPECT_TRUE(a3SentMessage.DataCount(), 2); 	// expect a3 to bundle previously unacked a2,a3

	// alice -[a4]->X bob
	const auto a4SentMessage = *alice.Send(a4); //bob.Receive(a3SentMessage);	
	EXPECT_TRUE(bob.receiveBuffer.Get(a4SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it
	EXPECT_TRUE(a3SentMessage.DataCount(), 3); 	// expect a3 to bundle previously unacked a2,a3, a4


	// alice -[a5]-> bob
	const auto a5SentMessage = *alice.Send(a5); bob.Receive(a5SentMessage);
	EXPECT_TRUE(a5SentMessage.DataCount(), 4); 	// expect a5 to bundle previously unacked a2,a3,a4 in a5

	// bob -[b1]-> alice
	const auto b1 = ReliableUdp::PacketDatum(false, "b1");
	const auto b1SentMessage = *bob.Send(b1); // Bob wil tell alice what it has received so far
	alice.Receive(b1SentMessage);

	// Bob's message should now be able to inform Alice that he received all were received

	EXPECT_TRUE(alice.sendBuffer.Get(a1SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a2SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a3SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a4SentMessage.Header.Sequence)->Acked);
	EXPECT_TRUE(alice.sendBuffer.Get(a5SentMessage.Header.Sequence)->Acked);
	
}