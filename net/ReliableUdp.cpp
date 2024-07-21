#include "ReliableUdp.h"
#include "GameClient.h"
#include "Option.h"

gamelib::Message* gamelib::ReliableUdp::MarkSent(PacketDatum datum, const bool isAckMessage)
{
	// increase sequence number for this datum to be sent
	datum.Sequence = ++sequence;

	// Prepare the package of data that will be sent starting with the datum that's intended to be sent
	std::vector dataToSent = { datum };		

	// add to send buffer
	SendBuffer.Put(sequence, datum);

	int messageSizeInBytes = ReliableUdpMessageHeader::GetSizeInBits() / 8;

	// Attach previous sequences' messages that have not been acked yet
	for(uint16_t i = 0; i < SendBuffer.GetBufferSize()-1; i++)
	{
		if(i == 0) continue; // We don't have a seq0 (sequence numbers start at 1)

		// Get previous sequence and see if its unacked...
		const auto pPreviousDatum = SendBuffer.Get(datum.Sequence - i);

		if(pPreviousDatum == nullptr) break;

		auto previousDatum = *pPreviousDatum;

		// We can only add up to the limit of what we can send. This is based on the MTU which is difficult to estimate so we use a
		// reasonable value that is around the general upper limit for MTUs which is 1000-1500 bytes.
		// We won't support packets larger than this (fragmentation) ie. splitting packets and reassembling them
		const auto canAddMoreData = messageSizeInBytes <= maxMessageSizeBytes - previousDatum.EstimateSizeInBytes();
				
		// We have some previous data that was not sent.
		if(!previousDatum.Acked && canAddMoreData)
		{
			// Add it to what must be sent with this message
			dataToSent.push_back(previousDatum);

			messageSizeInBytes += previousDatum.EstimateSizeInBytes();
		}
	}

	// Send Message, attaching any consecutive data that was not previously sent
	// also include a reference to what we've received from the sender previously, just in case an ack did not go through to the sender
	sentMessage = new Message(datum.Sequence, lastAckedSequence, GeneratePreviousAckedBits(), dataToSent.size(), dataToSent, isAckMessage);
			
	return sentMessage;
}

void gamelib::ReliableUdp::MarkReceived(const Message& senderMessage, unsigned long receivedTimeMs)
{
	// Sender received LastAckedSequence of ours we sent...		
	const auto* p = SendBuffer.Get(senderMessage.Header.LastAckedSequence);
	if(p != nullptr)
	{
		auto datum = *p;
		datum.Acked = true;
		SendBuffer.Put(datum.Sequence, datum);
	}

	// ...and the bits set in LastAckedBits should say how many prior sequences were also received
	for(uint16_t i = 0; i < static_cast<uint16_t>(sizeof (senderMessage.Header.LastAckedBits) * 8 -1) ; i++)
	{
		// if bit n is set, the prior sequence received was priorSeq - n+1
		if(BitFiddler<uint32_t>::BitCheck(senderMessage.Header.LastAckedBits, i))
		{
			// bit 0 means the previous previous sequence before LastAckedSequqnce was received, bit 1, the next prior, bit 2, the prior, prior
			const uint16_t previousSequence = (static_cast<uint16_t>(senderMessage.Header.LastAckedSequence) - (i+1)) ;
			auto datumToUpdate = *SendBuffer.Get(previousSequence);

			// mark it as having been received/acked by sender, acked messages will not be re-sent from the send buffer
			datumToUpdate.Acked = true;
			SendBuffer.Put(previousSequence, datumToUpdate);
		}
	}

	// Mark all the sequences as having been received
	for(int i = static_cast<int>(senderMessage.Data().size()) -1 ; i >= 0 ; i--)
	{
		// The current sequence is index 0, prior sequences were added on top, so a next index 1, 2, 3 are the prior sequences etc.
		auto datumToUpdate = senderMessage.Data()[i];
		datumToUpdate.Acked = true;
		ReceiveBuffer.Put(datumToUpdate.Sequence, datumToUpdate);
	}

	// last mark this as the last acknowledged sequence if we've not seen it before, i.e it larger than what we've previously seen
	if(senderMessage.Header.Sequence > lastAckedSequence)
	{
		lastAckedSequence = senderMessage.Header.Sequence;
	}
}

uint32_t gamelib::ReliableUdp::GeneratePreviousAckedBits()
{
	uint32_t previousAckedBits {};
	
	for(uint16_t i = 0; i < static_cast<uint16_t>(sizeof previousAckedBits * 8 -1); i++)
	{

		// If we received sequence x, we should also have received the prior sequences if the were no received because they were bundled up within x.
		// If the last 4 sequences prior to the lastAckedSequence(eg 10) was received, this is how we should format the bits:
		// 00000000000000000000000000001111
		// then this is understood to mean that 9, 8, 7, 6 were also acked
		const auto* pPriorDatum = ReceiveBuffer.Get(lastAckedSequence - (i+1));
		const auto bitPosition = i;
		
		if(pPriorDatum == nullptr) break;
		
		const auto currentPacket = *pPriorDatum;
		if(currentPacket.Acked)
		{				
			previousAckedBits = BitFiddler<uint32_t>::SetBit(previousAckedBits, bitPosition);
			continue;
		}		

		previousAckedBits = BitFiddler<uint32_t>::ClearBit(previousAckedBits, bitPosition);					
	}
	
	// NB: we'll tell the receiver that since the last acked message sequence, we also received the prior message sequence for each bit is set
	return previousAckedBits;
}
