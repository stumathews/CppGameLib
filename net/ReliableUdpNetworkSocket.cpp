#include "ReliableUdpNetworkSocket.h"

#include "Option.h"
#include "net/UdpConnectedNetworkSocket.h"

gamelib::ReliableUdpNetworkSocket::ReliableUdpNetworkSocket(const SOCKET socket): socket(socket)
{
	
}

bool gamelib::ReliableUdpNetworkSocket::IsTcp()
{
	return false;
}

int gamelib::ReliableUdpNetworkSocket::Send(const char* callersSendBuffer, const int dataLength)
{
	BitPacker packer(packingBuffer, PackingBufferElements);

	// Track/store message as sent
	const auto message = reliableUdp.MarkSent(PacketDatum(false, callersSendBuffer));

	// Write to the packingBuffer via the BitPacker	
	message->Write(packer); packer.Finish();

	// Count only as much as what was packed
	const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
	
	// Send only as much of the packedBuffer that was utilized
	return send(socket, reinterpret_cast<char*>(packingBuffer), countBytesToSend, 0);
}

int gamelib::ReliableUdpNetworkSocket::Receive(const char* callersReceiveBuffer, const int bufLength)
{	
	// Read the payload off the network, wait for all the data
	const int bytesReceived = recv(socket, const_cast<char*>(callersReceiveBuffer), bufLength, 0); 

	if(bytesReceived < 0)
	{
		return bytesReceived;
	}

	// Hook up to a 32-bit bitfield reader to the received buffer. This will read 32-bits at a time
	const auto count32BitBlocks = bytesReceived * 8 /32;
	
	BitfieldReader packedBufferReader(reinterpret_cast<uint32_t*>(readBuffer), count32BitBlocks);

	// Where the unpacked message will be stored
	Message message;

	// Unpack message
	message.Read(packedBufferReader);

	// Copy contents to caller's receive buffer to expose the contents of the message
	strcpy_s(const_cast<char*>(callersReceiveBuffer), bufLength, message.FirstPacket.c_str());

	return bytesReceived;
}

bool gamelib::ReliableUdpNetworkSocket::IsValid()
{
	return socket;
}

SOCKET gamelib::ReliableUdpNetworkSocket::GetRawSocket() const
{
	return socket;
}
