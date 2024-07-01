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

int gamelib::ReliableUdpNetworkSocket::Send(const char* data, const int dataLength)
{
	// create temporary buffer
	uint32_t buffer[8192]; // 256kb buffer

	// Get packet to send
	const auto message = reliableUdp.MarkSent(PacketDatum(false, data));

	// Pack packet tightly into  buffer
	BitPacker packer(buffer, 8192);
	message->Write(packer); packer.Finish();

	// Ask the client to sent only the bits we packed into the buffer
	const auto numBytes = ceil((double)packer.TotalBitsPacked() / (double)8);
	
	// Send over udp
	return send(socket, reinterpret_cast<char*>(buffer), numBytes, 0);
}

int gamelib::ReliableUdpNetworkSocket::Receive(const char* networkReadBuffer,
                                       const int bufLength)
{
	constexpr int maxBufferLength = 512;

	char readBuffer[maxBufferLength];
	constexpr int bufferLength = maxBufferLength;
	ZeroMemory(readBuffer, bufferLength);
	
	// Read the payload off the network, wait for all the data
	const int bytesReceived = recv(socket, const_cast<char*>(networkReadBuffer), bufLength, 0); 

	if(bytesReceived < 0)
	{
		return bytesReceived;
	}

	// Hook up bitfield reader to the received buffer
	BitfieldReader reader((uint32_t*)readBuffer, bytesReceived);

	Message m; m.Read(reader);
	libmonad::Option optionalMessage = m;
	
	strcpy_s((char*)networkReadBuffer, bufLength, m.FirstPacket.c_str());
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
