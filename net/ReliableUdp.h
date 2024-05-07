#pragma once
#include <cstdint>
#include <vector>

#include "utils/BitFiddler.h"
#include "utils/RingBuffer.h"

namespace gamelib
{
	

	class ReliableUdp
	{
	public:

		struct MessageHeader
		{
		    uint16_t Sequence;
		    uint16_t LastAckedSequence;
		    uint32_t LastAckedBits; // bit n is set means Sequence - n was acked
		};

		struct PacketDatum
		{
			PacketDatum(){ Acked = false; }
			explicit PacketDatum(const bool acked, const char* customData = "") : Acked(acked), CustomData(customData) {  }

		    bool Acked;
			uint16_t Sequence {};
			double SendTime {};
			const char* CustomData;

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

			MessageHeader Header{};
			[[nodiscard]] std::vector<PacketDatum> Data() const { return data; }
			[[nodiscard]] uint16_t DataCount() const { return data.size(); }
		private:
			std::vector<PacketDatum> data;
		};

		Message* Send(PacketDatum datum)
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
			sentMessage = new Message(sequence, lastAckedSequence, GeneratePreviousAckedBits(), dataToSent.size(), dataToSent.data());
			return sentMessage;
		}

		void Receive(const Message& message)
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

		uint32_t GeneratePreviousAckedBits()
		{
			uint32_t previousAckedBits {};

			// Bit 0 is always unset as it represents the current datm, while bit n represents (current - n) ie nth previous datum 
			previousAckedBits = BitFiddler<uint32_t>::ClearBit(previousAckedBits, 0);

			// Read the list of already received & acked data.
			for(uint32_t i = 0; i < sizeof previousAckedBits * 8 - 1; i++) // we only set the last 31 bits. Bit 1 is always 0 meaning current sending sequence is unacked
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
		
		RingBuffer<PacketDatum> receiveBuffer;
		RingBuffer<PacketDatum> sendBuffer;	
		uint16_t lastAckedSequence {};
	private:
		Message* sentMessage{};
		uint16_t sequence {};	
	};
}
