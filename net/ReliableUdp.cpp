#include "ReliableUdp.h"
#include "GameClient.h"
#include "Option.h"

gamelib::Message* gamelib::ReliableUdp::Send(const std::shared_ptr<GameClient> client, const PacketDatum datum)
{
	// create temporary buffer
	uint32_t buffer[8192]; // 256kb buffer

	// Get packet to send
	const auto message = MarkSent(datum);

	// Pack packet tightly into  buffer
	BitPacker packer(buffer, 8192);
	message->Write(packer);

	// Ask the client to sent only the bits we packed into the buffer
	client->SendBinary(reinterpret_cast<uint16_t*>(buffer), packer.TotalBitsPacked());

	return message;
}

int gamelib::ReliableUdp::Send(const std::shared_ptr<INetworkSocket>& networkSocket, const PacketDatum datum)
{
	// create temporary buffer
	uint32_t buffer[8192]; // 256kb buffer

	// Get packet to send
	const auto message = MarkSent(datum);

	// Pack packet tightly into  buffer
	BitPacker packer(buffer, 8192);
	message->Write(packer); packer.Finish();

	// Ask the client to sent only the bits we packed into the buffer
	const auto numBytes = ceil((double)packer.TotalBitsPacked() / (double)8);

	return networkSocket->Send(reinterpret_cast<char*>(buffer), numBytes);
}

libmonad::Option<gamelib::Message> gamelib::ReliableUdp::Receive(const std::shared_ptr<INetworkSocket>& networkSocket)
{
	constexpr int maxBufferLength = 512;

	char readBuffer[maxBufferLength];
	constexpr int bufferLength = maxBufferLength;
	ZeroMemory(readBuffer, bufferLength);
	
	// Read the payload off the network, wait for all the data
	const int bytesReceived = networkSocket->Receive(readBuffer, bufferLength);

	if(bytesReceived < 0)
	{
		return libmonad::None();
	}

	// Hook up bitfield reader to the received buffer
	BitfieldReader reader((uint32_t*)readBuffer, bytesReceived);

	Message m; m.Read(reader);
	libmonad::Option optionalMessage = m;
	
	return optionalMessage;
}

gamelib::Message* gamelib::ReliableUdp::MarkSent(PacketDatum datum)
{
	// increase sequence number for this datum to be sent
	datum.Sequence = ++sequence;

	// Prepare the package of data that will be sent starting with the datum that's intended to be sent
	std::vector dataToSent = { datum };		

	// add to send buffer
	sendBuffer.Put(sequence, datum);
			
	// We'll attach any unacked data that was supposed to have been sent previously
	for(uint16_t i = 0; i < sendBuffer.GetBufferSize(); i++)
	{
		if(i == 0) continue; // skip getting current sequence

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
	sentMessage = new Message(sequence, lastAckedSequence, GeneratePreviousAckedBits(), dataToSent.size(), dataToSent);
			
	return sentMessage;
}

void gamelib::ReliableUdp::MarkReceived(const Message& message)
{
	const auto messageSequence = message.Header.Sequence;
			
	// Mark the sender's last known message it acked from us as acked, i.e no need to resend 
	sendBuffer.Put(message.Header.LastAckedSequence, PacketDatum(true));
				
	// Look through the sender's list of previously acked messages and ensure that we dont re-send them
	constexpr uint16_t numBits = sizeof message.Header.LastAckedBits * 8;
	for(uint16_t i = 0; i < numBits ;i++)
	{
		if(i == 0) continue;

		if(BitFiddler<uint32_t>::BitCheck(message.Header.LastAckedBits, i))
		{
			const uint16_t previousSequence = (message.Header.LastAckedSequence - i) + 1;
			auto datumToUpdate = *sendBuffer.Get(previousSequence); datumToUpdate.Acked = true;
			sendBuffer.Put(previousSequence, datumToUpdate);
		}
	}

	// mark all the containing prior sequences as having been received too
	for(uint16_t i = 0; i < message.Data().size()-1; i++)
	{
		if(i == 0) continue; // skip current sequence

		const uint16_t currentSequence = messageSequence - i;
		auto datumToUpdate = message.Data()[currentSequence]; datumToUpdate.Acked = true;
		receiveBuffer.Put(currentSequence, datumToUpdate);
	}		
				
	// mark the this incoming sequence as finally received after all previous ones contained in the message
	PacketDatum incomingData = message.Data()[0]; incomingData.Acked = true;
	receiveBuffer.Put(messageSequence, incomingData);

	// last mark this as the last acknowledged sequence if we've not seen it before, i.e it larger than waht we've previously seen
	if(messageSequence > lastAckedSequence)
	{
		lastAckedSequence = messageSequence;
	}
}

uint32_t gamelib::ReliableUdp::GeneratePreviousAckedBits()
{
	uint32_t previousAckedBits {};

	// Bit 0 is always unset as it represents the current datm, while bit n represents (current - n) ie nth previous datum 
	previousAckedBits = BitFiddler<uint32_t>::ClearBit(previousAckedBits, 0);

	// Read the list of already received & acked data.
	for(uint16_t i = 0; i < sizeof previousAckedBits * 8 - 1; i++) // we only set the last 31 bits. Bit 1 is always 0 meaning current sending sequence is unacked
	{
		const auto* pCurrentDatum = receiveBuffer.Get(lastAckedSequence - i);
		const auto bitPosition = i + 1; // offset bit position by 1 as we've already

		if(pCurrentDatum != nullptr)
		{
			const auto currentPacket = *pCurrentDatum;
			if(currentPacket.Acked)
			{				
				previousAckedBits = BitFiddler<uint32_t>::SetBit(previousAckedBits, bitPosition);
				continue;
			}
		}

		previousAckedBits = BitFiddler<uint32_t>::ClearBit(previousAckedBits, bitPosition);					
	}
			
	// NB: we'll tell the receiver that this is what we've already received
	return previousAckedBits;
}
