#include "ReliableUdpGameServerConnection.h"
#include <net/Networking.h>
#include <events/EventManager.h>
#include <net/PeerInfo.h>

#include "events/EventFactory.h"
#include "objects/GameObject.h"

namespace gamelib
{
	ReliableUdpGameServerConnection::ReliableUdpGameServerConnection(const std::string& host, const std::string& port):
		UdpGameServerConnection(host, port)
	{
	}
	
	void ReliableUdpGameServerConnection::CheckForPlayerTraffic(const unsigned long deltaMs)
	{
		// Identifier who the client sending the data is
		PeerInfo fromClient;
						
		// Read all incoming data into readBuffer
		const int bytesReceived = recvfrom(listeningSocket, reinterpret_cast<char*>(readBuffer), ReadBufferSizeInBytes, 0,
		                                   reinterpret_cast<sockaddr*>(&fromClient.Address), &fromClient.Length);

		const auto numReceivedReadBufferElements = bytesReceived * 8 /32;

		if (bytesReceived > 0)
		{
			// Hook up to a 32-bit bitfield reader to the received buffer. This will read 32-bits at a time			
			BitfieldReader reader(readBuffer, numReceivedReadBufferElements);
						
			// Unpack receive buffer into message
			Message message;
			message.Read(reader);

			// Check for any transmission errors
			if(!message.ValidateChecksum())
			{
				RaiseEvent(EventFactory::Get()->CreateReliableUdpCheckSumFailedEvent(std::make_shared<Message>(message)));

				// We drop the packet, no acknowledgment is sent. Sender will need to resend.
				return;
			}


			// Mark this sequence as having been received
			reliableUdp.MarkReceived(message, deltaMs);

			// Send a acknowledgment that normal message was received (dont send acks to acks)
			if(message.Header.MessageType != 0)
			{				
				RaiseEvent(EventFactory::Get()->CreateReliableUdpPacketReceived(std::make_shared<Message>(message)));
				SendAck(listeningSocket, message, 0, fromClient, deltaMs);
			}

			// For the case were received multiple messages (aggregate)
			// The first message payload is the message we've just received, the following payloads
			// are for the previous messages that were bundled with this message.
			// ie data[0] = current, data[1] is prior, data[2] is prior prior. So we need to process in reverse order until data[0]
			const auto messageData = message.Data();
			for(int i = static_cast<int>(message.Data().size()) -1 ; i >= 0 ; i--)
			{
				const PacketDatum& datum = messageData[i];
				const char* payloadData = datum.Data();
				const int dataSize = static_cast<int>(strlen(payloadData));

				// Temporarily do not send others states to other players
				//ParseReceivedPlayerPayload(payloadData, dataSize, fromClient);
				RaiseNetworkTrafficReceivedEvent(payloadData, dataSize, fromClient);
			}
		}
		else
		{			
			Networking::Get()->netError(bytesReceived, WSAGetLastError(), "Error listening for player traffic");
		}
	}
	
	int ReliableUdpGameServerConnection::InternalSend(const SOCKET socket, const char* buf, int len, const int flags,  const sockaddr* to, const int toLen, const unsigned long sendTimeMs)
	{		
		BitPacker packer(readBuffer, ReadBufferMaxElements);
				
		// Prepare data to be sent
		const auto data = PacketDatum(false, buf, sendTimeMs);		

		// Add data to message and mark message as having been sent (we sent it later)
		const auto message = reliableUdp.MarkSent(data );

		message->Header.MessageType = 1; // non-ack

		// Write message to network buffer		
		message->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		// Send network buffer over udp		
		return sendto(socket, reinterpret_cast<char*>(readBuffer), countBytesToSend, flags, to, toLen);
	}

	int ReliableUdpGameServerConnection::SendAck(const SOCKET socket, const Message& messageToAck, const int flags, PeerInfo& peerInfo, const unsigned long sendTimeMs)
	{
		BitPacker packer(readBuffer, ReadBufferMaxElements);

		std::stringstream ackMessage;
		ackMessage << "server acks client seq:" << messageToAck.Header.Sequence << std::endl;

		// Add data to message and mark message as having been sent (we sent it later).
		// Note we explicitly acked=true this to avoid resending it (acks are fire and forgets)
		const auto message = reliableUdp.MarkSent(PacketDatum(true, ackMessage.str().c_str()), sendTimeMs);

		message->Header.MessageType = 0; // acknowledgment

		// Write message to network buffer		
		message->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		// Send network buffer over udp		
		return sendto(socket, reinterpret_cast<char*>(readBuffer), countBytesToSend, flags, reinterpret_cast<sockaddr*>(&peerInfo.Address), peerInfo.Length);
	}
}



