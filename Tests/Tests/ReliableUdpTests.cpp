
#include "pch.h"

#include <gtest/gtest.h>
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

	const auto data1 = PacketDatum( false, "data1");
	const auto data2 = PacketDatum( false, "data2");
	const auto data3 = PacketDatum( false, "data3");
		
	// Send

	const auto* message1 = reliableUdp.MarkSent(data1);	
	EXPECT_EQ(reliableUdp.SendBuffer.GetLastAddedSequence(), message1->Header.Sequence);

	const auto* message2 = reliableUdp.MarkSent(data2);	
	EXPECT_EQ(reliableUdp.SendBuffer.GetLastAddedSequence(), message2->Header.Sequence);

	const auto* message3 = reliableUdp.MarkSent(data3);	
	EXPECT_EQ(reliableUdp.SendBuffer.GetLastAddedSequence(), message3->Header.Sequence);
	
	EXPECT_EQ(message1->DataCount(), 1);
	EXPECT_EQ(message1->Header.Sequence, 1);
	EXPECT_STREQ(message1->Data()[0].Data(), data1.Data());
	EXPECT_EQ(message1->Header.LastAckedBits, 0);
	EXPECT_EQ(message1->Header.LastAckedSequence, 0);
	
	EXPECT_EQ(message2->DataCount(), 2);
	EXPECT_EQ(message2->Header.Sequence, 2);
	EXPECT_EQ(message2->Header.LastAckedBits, 0);
	EXPECT_EQ(message2->Header.LastAckedSequence, 0);

	// Should also contain message 1 as it it not acked
	EXPECT_STREQ(message2->Data()[0].Data(), data2.Data());
	EXPECT_STREQ(message2->Data()[1].Data(), data1.Data());

	EXPECT_FLOAT_EQ(message3->DataCount(), 3);
	EXPECT_EQ(message3->Header.Sequence, 3);
	EXPECT_EQ(message3->Header.LastAckedBits, 0);
	EXPECT_EQ(message3->Header.LastAckedSequence, 0);

	// should also contain message1, message2 as they are also not acked
	EXPECT_STREQ(message3->Data()[0].Data(), data3.Data());
	EXPECT_STREQ(message3->Data()[1].Data(), data2.Data());
	EXPECT_STREQ(message3->Data()[2].Data(), data1.Data());

}

TEST_F(ReliableUdpTests, BasicRecieve)
{
	ReliableUdp reliableUdp;

	const auto data1 = PacketDatum(false, "data1");
	const auto data2 = PacketDatum(false, "data2");
	const auto data3 = PacketDatum(false, "data3");

	const auto* message1 = reliableUdp.MarkSent(data1);
	reliableUdp.MarkReceived(*message1);

	EXPECT_EQ(reliableUdp.ReceiveBuffer.GetLastAddedSequence(), message1->Header.Sequence);
	EXPECT_EQ(reliableUdp.lastAckedSequence, message1->Header.Sequence);
	EXPECT_TRUE(reliableUdp.ReceiveBuffer.Get(message1->Header.Sequence)->IsAcked);

	const auto* message2 = reliableUdp.MarkSent(data2);
	reliableUdp.MarkReceived(*message2);

	EXPECT_EQ(reliableUdp.ReceiveBuffer.GetLastAddedSequence(), message2->Header.Sequence);//
	EXPECT_EQ(reliableUdp.lastAckedSequence, message2->Header.Sequence);
	EXPECT_TRUE(reliableUdp.ReceiveBuffer.Get(message2->Header.Sequence)->IsAcked);

	const auto* message3 = reliableUdp.MarkSent(data3);
	reliableUdp.MarkReceived(*message3);

	EXPECT_EQ(reliableUdp.ReceiveBuffer.GetLastAddedSequence(), message3->Header.Sequence);//
	EXPECT_EQ(reliableUdp.lastAckedSequence, message3->Header.Sequence);
	EXPECT_TRUE(reliableUdp.ReceiveBuffer.Get(message3->Header.Sequence)->IsAcked);

	auto senderAckedBits = reliableUdp.GeneratePreviousAckedBits();

	EXPECT_TRUE(BitFiddler<uint32_t>::BitCheck(senderAckedBits, 0)); // if bit n is set, means the nth prior sequence was received eg. 3-0 = sequence 3
	EXPECT_TRUE(BitFiddler<uint32_t>::BitCheck(senderAckedBits, 1));  // if bit n is set, means the nth prior sequence was received eg. 3-1 = sequence 2
	EXPECT_FALSE(BitFiddler<uint32_t>::BitCheck(senderAckedBits, 2));  // if bit n is set, means the nth prior sequence was received eg. 3-2 = sequence 1
}

TEST_F(ReliableUdpTests, AliceBobBasic)
{
	ReliableUdp alice;
	ReliableUdp bob;

	// alice -[a1]-> bob:

	const auto a1 = PacketDatum( false, "a1");

	// Send a1 to bob along with any indications that alice has received previously sent from bob,
	// which would be none as bob as not sent alice anything yet
	const auto a1SentMessage = *alice.MarkSent(a1);

	// bob receives it and marks that it was received. The next message sent to alice will inform alice that it was received
	bob.MarkReceived(a1SentMessage);

	// As nothing from bob has been sent to alice to inform alice that bob received anything from alice, alice should still consider what it sent
	// as having not been acknowledged
	EXPECT_FALSE(alice.SendBuffer.Get(a1SentMessage.Header.Sequence)->IsAcked);

	// bob -[a1ack]-> alice:

	const auto b1 = PacketDatum( false, "b1");

	// Bob will send something to alice, and say that it knows about the last thing alice sent him, which if it happens to be what alice sent him
	// which it it, alice can mark off that it was acked
	const auto sentA1AckMessage = *bob.MarkSent(b1);

	alice.MarkReceived(sentA1AckMessage);

	// Expect that that bob's mesage header contained the last ack sequence, which alice removed from alice's send buffer,
	EXPECT_TRUE(alice.SendBuffer.Get(a1SentMessage.Header.Sequence)->IsAcked);

	// Ensure alice does not send it again
	const auto a2 = PacketDatum(false, "a2");
	const auto a2SentMessage = alice.MarkSent(a2);
	EXPECT_EQ(a2SentMessage->DataCount(), 1);
	EXPECT_EQ(a2SentMessage->Data()[0].Sequence, a2SentMessage->Header.Sequence);
}

TEST_F(ReliableUdpTests, AliceBobAggregateMessagesDrop1)
{
	const auto a1 = PacketDatum(false, "a1");
	const auto a2 = PacketDatum(false, "a2");
	const auto a3 = PacketDatum(false, "a3");

	ReliableUdp alice;
	ReliableUdp bob;

	// alice -[a1]-> bob
	const auto a1SentMessage = *alice.MarkSent(a1); bob.MarkReceived(a1SentMessage);
	EXPECT_TRUE(bob.ReceiveBuffer.Get(a1SentMessage.Header.Sequence)->IsAcked);

	// alice -[a2]->X bob
	const auto a2SentMessage = *alice.MarkSent(a2); //bob.Receive(a2SentMessage); // simulate bob not receiving it
	EXPECT_TRUE(bob.ReceiveBuffer.Get(a2SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it

	// alice -[a3]-> bob
	const auto a3SentMessage = *alice.MarkSent(a3); bob.MarkReceived(a3SentMessage);
	EXPECT_TRUE(bob.ReceiveBuffer.Get(a2SentMessage.Header.Sequence)->IsAcked); // this time we got it, because it was sent as part of message3 (it was sent again)
	EXPECT_TRUE(bob.ReceiveBuffer.Get(a3SentMessage.Header.Sequence)->IsAcked);

	// bob -[b1]-> alice
	const auto b1 = PacketDatum(false, "b1");
	const auto b1SentMessage = *bob.MarkSent(b1); // Bob wil tell alice what it has received so far
	alice.MarkReceived(b1SentMessage);

	// Bob's message should now be able to inform Alice that he received a1 a2 a3 were received

	EXPECT_TRUE(alice.SendBuffer.Get(a1SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a2SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a3SentMessage.Header.Sequence)->IsAcked);
	
}

TEST_F(ReliableUdpTests, AliceBobAggregateMessagesDrop3)
{
	const auto a1 = PacketDatum(false, "a1");
	const auto a2 = PacketDatum(false, "a2");
	const auto a3 = PacketDatum(false, "a3");
	const auto a4 = PacketDatum(false, "a4");
	const auto a5 = PacketDatum(false, "a5");

	ReliableUdp alice;
	ReliableUdp bob;

	// alice -[a1]-> bob
	const auto a1SentMessage = *alice.MarkSent(a1);
	bob.MarkReceived(a1SentMessage);

	// alice -[a2]->X bob
	const auto a2SentMessage = *alice.MarkSent(a2); //bob.Receive(a2SentMessage); // simulate bob not receiving it
	EXPECT_TRUE(bob.ReceiveBuffer.Get(a2SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it

	// alice -[a3]->X bob
	const auto a3SentMessage = *alice.MarkSent(a3); //bob.Receive(a3SentMessage);	
	EXPECT_TRUE(bob.ReceiveBuffer.Get(a3SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it
	EXPECT_EQ(a3SentMessage.DataCount(), 3); 	// expect a3 to bundle previously unacked a1, a2,a3

	// alice -[a4]->X bob
	const auto a4SentMessage = *alice.MarkSent(a4); //bob.Receive(a3SentMessage);	
	EXPECT_TRUE(bob.ReceiveBuffer.Get(a4SentMessage.Header.Sequence) == nullptr); // ensure bob didn't get it
	EXPECT_EQ(a4SentMessage.DataCount(), 4); 	// expect a3 to bundle previously unacked a1,a2,a3, a4
	
	// alice -[a5]-> bob
	const auto a5SentMessage = *alice.MarkSent(a5);
	bob.MarkReceived(a5SentMessage);
	EXPECT_EQ(a5SentMessage.DataCount(), 5); 	// expect a5 to bundle previously unacked a2,a3,a4 in a5

	// bob -[b1]-> alice
	const auto b1 = PacketDatum(false, "b1");
	const auto b1SentMessage = *bob.MarkSent(b1); // Bob wil tell alice what it has received so far
	alice.MarkReceived(b1SentMessage);

	// Bob's message should now be able to inform Alice that he received all were received

	EXPECT_TRUE(alice.SendBuffer.Get(a1SentMessage.Header.Sequence)->IsAcked); //
	EXPECT_TRUE(alice.SendBuffer.Get(a2SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a3SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a4SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a5SentMessage.Header.Sequence)->IsAcked);
	
}

TEST_F(ReliableUdpTests, AliceBobAckTest)
{
	const auto a1 = PacketDatum(false, "a1");
	const auto a2 = PacketDatum(false, "a2");
	const auto a3 = PacketDatum(false, "a3");

	ReliableUdp alice;
	ReliableUdp bob;

	// alice -[a1]-> bob
	const auto a1SentMessage = *alice.MarkSent(a1);
	// alice send buffer [seq1:a1, unack, senderLastAcked=0]

	bob.MarkReceived(a1SentMessage);
	// bob send buffer [seq0:, ack, sender(alice)LastAcked=0]
	// sender(alice)Message shows no previously acked messages to add to our send buffer as acked

	// alice -[a2]-> bob
	const auto a2SentMessage = *alice.MarkSent(a2);
	// alice send buffer [seq1:a1, unack]
	//                   [seq2:a2, unack]

	// alice -[a3]-> bob
	const auto a3SentMessage = *alice.MarkSent(a3);
	// alice send buffer [seq1:a1, unack]
	//                   [seq2:a2, unack]
	//                   [seq3:a3, unack]

	bob.MarkReceived(a3SentMessage);
	// bob send buffer [seq0:, ack, sender(alice)LastAcked=0]
	// bob recv buffer [seq1:a1]
	//                 [seq2:a2]
	//                 [seq3:a3]
	// sender(alice)Message shows no previously acked messages to add to our send buffer as acked
	
	// bob -[b1]-> alice

	const auto b1 = PacketDatum(false, "b1");
	const auto b1SentMessage = *bob.MarkSent(b1); // Bob wil tell alice what it has received so far (a1, a2, a3)
	// bob sendbuffer [seq0:, ack, sender(alice)LastAcked=0
	//                [seq1: b1, unack, bits:111]

	EXPECT_EQ(b1SentMessage.Header.LastAckedSequence, a3SentMessage.Header.Sequence);
	EXPECT_STREQ(BitFiddler<uint32_t>::ToString(b1SentMessage.Header.LastAckedBits).c_str(), "00000000000000000000000000000011"); // the two prior sequences were received, not including the current sequence being sent

	alice.MarkReceived(b1SentMessage);
	// alice send buffer [seq1:a1, ack]
	//                   [seq2:a2, ack]
	//                   [seq3:a3, ack]
	
	EXPECT_TRUE(alice.SendBuffer.Get(a1SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a2SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a3SentMessage.Header.Sequence)->IsAcked);
}

TEST_F(ReliableUdpTests, AliceBobOutOfOrder)
{
	const auto a1 = PacketDatum(false, "a1");
	const auto a2 = PacketDatum(false, "a2");
	const auto a3 = PacketDatum(false, "a3");

	ReliableUdp alice;
	ReliableUdp bob;

	// alice -[a1]-> bob
	const auto a1SentMessage = *alice.MarkSent(a1); bob.MarkReceived(a1SentMessage);

	// alice -[a2]-> bob
	const auto a2SentMessage = *alice.MarkSent(a2); 

	// alice -[a3]-> bob
	const auto a3SentMessage = *alice.MarkSent(a3);

	// receive out of order
	bob.MarkReceived(a3SentMessage);
	bob.MarkReceived(a2SentMessage);

	// bob -[b1]-> alice
	const auto b1 = PacketDatum(false, "b1");
	const auto b1SentMessage = *bob.MarkSent(b1); // Bob wil tell alice what it has received so far
	alice.MarkReceived(b1SentMessage);

	// Ensure out of order messages do not cause a problem, i.e ordered reliable transmission is guaranteed

	EXPECT_TRUE(alice.SendBuffer.Get(a1SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a2SentMessage.Header.Sequence)->IsAcked);
	EXPECT_TRUE(alice.SendBuffer.Get(a3SentMessage.Header.Sequence)->IsAcked);
}