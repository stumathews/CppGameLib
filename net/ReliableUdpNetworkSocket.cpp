#include "ReliableUdpNetworkSocket.h"

#include "Networking.h"
#include "Option.h"
#include "events/EventFactory.h"

gamelib::ReliableUdpNetworkSocket::ReliableUdpNetworkSocket(const SOCKET socket): socket(socket)
{}

gamelib::ReliableUdpNetworkSocket::ReliableUdpNetworkSocket() = default;

bool gamelib::ReliableUdpNetworkSocket::IsTcp() { return false; }

int gamelib::ReliableUdpNetworkSocket::Send(const char* callersSendBuffer, const int dataLength, const unsigned long deltaMs)
{
	BitPacker packer(packingBuffer, PackingBufferElements);

	// Track/store message as sent
	const auto message = reliableUdp.MarkSent(PacketDatum(false, callersSendBuffer, deltaMs), false);

	// Packet loss detected as we have unacknowledged data in send buffer that we are resending
	if(message->DataCount() > 1)
	{		
		this->RaiseEvent(EventFactory::Get()->CreateReliableUdpPacketLossDetectedEvent(std::make_shared<Message>(*message)));
	}

	// Write to the packingBuffer via the BitPacker	
	message->Write(packer);

	// Count only as much as what was packed
	const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
	
	// Send only as much of the packedBuffer that was utilized
	return send(socket, reinterpret_cast<char*>(packingBuffer), countBytesToSend, 0);
}

int gamelib::ReliableUdpNetworkSocket::Receive(char* callersReceiveBuffer, const int bufLength, unsigned long deltaMs)
{	
	// Read the payload off the network, wait for all the data
	const int bytesReceived = recv(socket, callersReceiveBuffer, bufLength, 0); 

	if(bytesReceived < 0)
	{
		return bytesReceived;
	}

	// Hook up to a 32-bit bitfield reader to the received buffer. This will read 32-bits at a time
	const auto count32BitBlocks = bytesReceived * 8 /32;
	
	BitfieldReader packedBufferReader(reinterpret_cast<uint32_t*>(callersReceiveBuffer), count32BitBlocks);

	// Where the unpacked message will be stored
	Message message;

	// Unpack message
	message.Read(packedBufferReader);

	// Check for any transmission errors
	if(!message.ValidateChecksum())
	{
		RaiseEvent(EventFactory::Get()->CreateReliableUdpCheckSumFailedEvent(std::make_shared<Message>(message)));

		// We drop the packet, no acknowledgment is sent. Sender will need to resend.
		return 0;
	}

	reliableUdp.MarkReceived(message, deltaMs);

	// SendAck - only to non-ack messages (don't ack an ack!)
	if(message.Header.MessageType != 0)	
	{
		
		RaiseEvent(EventFactory::Get()->CreateReliableUdpPacketReceived(std::make_shared<Message>(message)));
		SendAck(message, deltaMs);
	}

	// Copy contents to caller's receive buffer to expose the contents of the message
	std::stringstream ss;
	for(auto& packet : message.Data())
	{
		ss << packet.Data();
	}
		
	strcpy_s(callersReceiveBuffer, bufLength, ss.str().c_str());

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

int gamelib::ReliableUdpNetworkSocket::SendAck(const Message& messageToAck, const unsigned long sendTimeMs)
{		
	BitPacker packer(packingBuffer, PackingBufferElements);

	std::stringstream ackMessageContents;
		ackMessageContents << "client acks server's seq:" << messageToAck.Header.Sequence << std::endl;

	// Prepare data to be sent - not ack will be pre-acked so we don't try to resend it
	const auto data = PacketDatum(true, ackMessageContents.str().c_str(), sendTimeMs);		

	// Add data to message and mark message as having been sent (we sent it later)
	auto* ackMessage = reliableUdp.MarkSent(data, true);

	ackMessage->Header.MessageType = 0; // ack

	// Write message to network buffer		
	ackMessage->Write(packer);

	// Count only as much as what was packed
	const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
	
	// Send network buffer over udp		
	return send(socket, reinterpret_cast<char*>(readBuffer), countBytesToSend, 0);
}

void gamelib::ReliableUdpNetworkSocket::Connect(const char* address, const char* port)
{
	auto* networking = Networking::Get();
	socket = networking->netConnectedUdpClient(address, port);
}

std::string gamelib::ReliableUdpNetworkSocket::GetSubscriberName()
{
	return "ReliableUdpNetworkSocket";
}

std::vector<std::shared_ptr<gamelib::Event>> gamelib::ReliableUdpNetworkSocket::HandleEvent(
	const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
{
	// handling no events yet
	return {};
}

int gamelib::ReliableUdpNetworkSocket::GetSubscriberId()
{
	return EventSubscriber::GetSubscriberId();
}


