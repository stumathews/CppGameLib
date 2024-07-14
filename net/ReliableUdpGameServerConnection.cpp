#include "ReliableUdpGameServerConnection.h"
#include <net/Networking.h>
#include <events/EventManager.h>
#include <net/PeerInfo.h>

namespace gamelib
{
	ReliableUdpGameServerConnection::ReliableUdpGameServerConnection(const std::string& host, const std::string& port):
		UdpGameServerConnection(host, port)
	{
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

			// Mark this sequence as having been received
			reliableUdp.MarkReceived(message);

			// Send a acknowledgment that message was received
			SendAck(message, fromClient);

			// For the case were received multiple messages (aggregate)
			for( auto& payload : message.Data())
			{
				const char* payloadData = payload.Data();
				const int dataSize = static_cast<int>(strlen(payloadData));

				ParseReceivedPlayerPayload(payloadData, dataSize, fromClient);
				RaiseNetworkTrafficReceivedEvent(payloadData, dataSize, fromClient);
			}
		}
	}

	void ReliableUdpGameServerConnection::SendAck(const Message& message, PeerInfo& to)
	{
		std::stringstream ackMessage;
		ackMessage << "server ack seq:" << message.Header.Sequence;
		
		InternalSend(listeningSocket, ackMessage.str().c_str(), static_cast<int>(strlen(ackMessage.str().c_str())), 0,
		             reinterpret_cast<sockaddr*>(&to.Address), to.Length);
	}

	
	int ReliableUdpGameServerConnection::InternalSend(const SOCKET socket, const char* buf, int len, const int flags,  const sockaddr* to, int toLen)
	{		
		BitPacker packer(networkBuffer, NetworkBufferSize);
				
		// Prepare data to be sent
		const auto data = PacketDatum(false, buf);

		// Add data to message and mark message as having been sent (we sent it later)
		const auto message = reliableUdp.MarkSent(data);

		// Write message to network buffer		
		message->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		// Send network buffer over udp		
		return sendto(socket, reinterpret_cast<char*>(networkBuffer), countBytesToSend, flags, to, toLen);
	}

	
}



