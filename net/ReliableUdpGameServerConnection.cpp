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

	std::vector<std::shared_ptr<Event>> ReliableUdpGameServerConnection::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{
		return {};
	}

	void ReliableUdpGameServerConnection::CheckForPlayerTraffic()
	{
		// Identifier who the client sending the data is
		PeerInfo fromClient; 
				
		// Read all incoming data into readBuffer		
		const int bytesReceived = recvfrom(listeningSocket, receiveBuffer, ReceiveBufferMaxElements, 0,
		                                   reinterpret_cast<sockaddr*>(&fromClient.Address), &fromClient.Length);


		if (bytesReceived > 0)
		{
			// Hook up to a 32-bit bitfield reader to the received buffer. This will read 32-bits at a time

			const auto count32BitBlocks = bytesReceived * 8 /32;
			BitfieldReader reader(reinterpret_cast<uint32_t*>(receiveBuffer), count32BitBlocks);
						
			// Unpack receive buffer into message
			Message message; message.Read(reader);

			RaiseEvent(EventFactory::Get()->CreateReliableUdpPacketReceived(std::make_shared<Message>(message)));

			// Mark this sequence as having been received
			reliableUdp.MarkReceived(message);

			// Send a acknowledgment that message was received
			SendAck(listeningSocket, message, 0, fromClient);

			// For the case were received multiple messages (aggregate)
			for( auto& payload : message.Data())
			{
				const char* payloadData = payload.Data();
				const int dataSize = static_cast<int>(strlen(payloadData));

				ParseReceivedPlayerPayload(payloadData, dataSize, fromClient);
				RaiseNetworkTrafficReceivedEvent(payloadData, dataSize, fromClient);
			}
		}
		else
		{			
			Networking::Get()->netError(bytesReceived, WSAGetLastError(), "Error listening for player traffic");
		}
	}

	
	int ReliableUdpGameServerConnection::InternalSend(const SOCKET socket, const char* buf, int len, const int flags,  const sockaddr* to, int toLen)
	{		
		BitPacker packer(networkBuffer, NetworkBufferSize);
				
		// Prepare data to be sent
		const auto data = PacketDatum(false, buf);		

		// Add data to message and mark message as having been sent (we sent it later)
		const auto message = reliableUdp.MarkSent(data);

		message->Header.MessageType = 1; // non-ack

		// Write message to network buffer		
		message->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		// Send network buffer over udp		
		return sendto(socket, reinterpret_cast<char*>(networkBuffer), countBytesToSend, flags, to, toLen);
	}

	int ReliableUdpGameServerConnection::SendAck(const SOCKET socket, const Message& messageToAck, const int flags, PeerInfo& peerInfo)
	{
		BitPacker packer(networkBuffer, NetworkBufferSize);

		std::stringstream ackMessage;
		ackMessage << "server acks seq:" << messageToAck.Header.Sequence << std::endl;

		// Add data to message and mark message as having been sent (we sent it later).
		// Note we explicitly acked=true this to avoid resending it (acks are fire and forgets)
		const auto message = reliableUdp.MarkSent(PacketDatum(true, ackMessage.str().c_str()));

		message->Header.MessageType = 0; // acknowledgment

		// Write message to network buffer		
		message->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		// Send network buffer over udp		
		return sendto(socket, reinterpret_cast<char*>(networkBuffer), countBytesToSend, flags, reinterpret_cast<sockaddr*>(&peerInfo.Address), peerInfo.Length);
	}
}



