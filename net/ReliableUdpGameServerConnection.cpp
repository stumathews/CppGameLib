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

			// Where to store the message
			Message message;

			// Unpack receive buffer into message
			message.Read(reader);
			reliableUdp.MarkReceived(message);

			// for the case were received multiple messages (aggregate)
			for( auto& payload : message.Data())
			{
				const char* data = payload.Data();
				ParseReceivedPlayerPayload(data, strlen(data), fromClient);
				RaiseNetworkTrafficReceivedEvent(data, strlen(data), fromClient);
			}
		}
	}

	
	int ReliableUdpGameServerConnection::InternalSend(const SOCKET socket, const char* buf, int len, const int flags,  const sockaddr* to, int toLen)
	{
		// Get packet to send
		const auto message = reliableUdp.MarkSent(PacketDatum(false, buf));
		
		BitPacker packer(packingBuffer, PackingBufferElements);

		// Pack packet tightly into  buffer before sending
		message->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		// Send over udp		
		return sendto(socket, reinterpret_cast<char*>(packingBuffer), countBytesToSend, flags, to, toLen);
	}

	
}



