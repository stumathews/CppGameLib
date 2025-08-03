#include "ReliableUdpGameServerConnection.h"
#include <net/Networking.h>
#include <events/EventManager.h>
#include <net/PeerInfo.h>
#include "PacketDatumUtils.h"
#include "events/EventFactory.h"
#include "file/Logger.h"
#include "objects/GameObject.h"
#include "events/ReliableUdpCheckSumFailedEvent.h"
#include "events/ReliableUdpAckPacketEvent.h"
#include "events/ReliableUdpPacketRttCalculatedEvent.h"
#include "events/ReliableUdpPacketReceivedEvent.h"

namespace gamelib
{
	ReliableUdpGameServerConnection::ReliableUdpGameServerConnection(const std::string& host, const std::string& port,
	                                                                 const bool useEncryption,
	                                                                 const gamelib::Encoding wireFormat):
		UdpGameServerConnection(host, port, wireFormat), useEncryption(useEncryption)
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

		if(sessionEstablished && useEncryption)
		{
			Logger::Get()->LogThis("Server: Secure session successfully established.");

			// We will use the same nonce for now (constant)
			*sharedNonce = 1;

			// We'll store the decrypted message minus the auth tag here
			std::vector<unsigned char> decryptedMessage(bytesReceived - crypto_secretbox_MACBYTES);

			// decrypt message
			const auto decryptSucceeded = Security::Get()->DecryptWithSessionKey(
				reinterpret_cast<const unsigned char*>(readBuffer),
				bytesReceived,
				securitySide.GetReceiveSessionKey(), 
				sharedNonce,
				decryptedMessage.data());

			if(decryptSucceeded < 0)
			{
				Logger::Get()->LogThis("Server: Decrypt failed.");
				return;
			}

			Logger::Get()->LogThis("Server: Successfully decrypted message.");

			// copy decrypted message into caller's buffer
			memcpy_s(readBuffer, ReadBufferSizeInBytes, decryptedMessage.data(), decryptedMessage.size());
			
		}
		else
		{
			Logger::Get()->LogThis("Server: [Secure session not yet established]");
		}

		if (bytesReceived > 0)
		{
			Logger::Get()->LogThis("Server: Received " + std::to_string(bytesReceived) + " bytes");

			// Hook up to a 32-bit bitfield reader to the received buffer. This will read 32-bits at a time			
			BitfieldReader reader(readBuffer, numReceivedReadBufferElements);
						
			// Unpack receive buffer into message
			Message message;
			message.Read(reader);

			// Check for any transmission errors
			if(!message.ValidateChecksum())
			{
				Logger::Get()->LogThis("Server: Received invalid checksum");

				RaiseEvent(EventFactory::Get()->CreateReliableUdpCheckSumFailedEvent(std::make_shared<Message>(message)));
								
				// We drop the packet, no acknowledgment is sent. Sender will need to resend.
				return;
			}
			
			// Mark this sequence as having been received
			reliableUdp.MarkReceived(message, deltaMs);			

			// Message handling:

			Logger::Get()->LogThis("Server: [Received "+ MessageTypeToString(message.Header.MessageType) +"]");				

			if(message.Header.MessageType == Ack) // ack message
			{
				RaiseEvent(EventFactory::Get()->CreateReliableUdpAckPacketEvent(std::make_shared<Message>(message), false));				
				
				RaiseEvent(EventFactory::Get()->CreateReliableUdpPacketRttCalculatedEvent(std::make_shared<Message>(message), 
					PacketDatumUtils::CalculateRttStats(message, reliableUdp.ReceiveBuffer)));
				
			}

			if (message.Header.MessageType != Ack) // non-ack message
			{
				RaiseEvent(EventFactory::Get()->CreateReliableUdpPacketReceived(std::make_shared<Message>(message)));

				// For the case where we've received multiple messages (aggregate).

				// The first message payload is the message we've just received, the following payloads
				// are for the previous messages that were bundled with this message.

				// ie data[0] = current, data[1] is prior, data[2] is prior prior. So we need to process in reverse order until data[0]

				const auto messageData = message.Data();
				for (int i = static_cast<int>(message.Data().size()) - 1; i >= 0; i--)
				{
					const PacketDatum& datum = messageData[i];
					const char* payloadData = datum.Data();
					const int dataSize = static_cast<int>(strlen(payloadData));

					// Temporarily do not send others states to other players
					//ParseReceivedPlayerPayload(payloadData, dataSize, fromClient);
					RaiseNetworkTrafficReceivedEvent(payloadData, dataSize, fromClient);
				}

				SendAck(listeningSocket, message, 0, fromClient, deltaMs);

				// sent ack
				RaiseEvent(EventFactory::Get()->CreateReliableUdpAckPacketEvent(std::make_shared<Message>(message), true));
			}

			if (message.Header.MessageType == SendPubKey)
			{
				// We got the client's public key
				memcpy_s(remotePublicKey, SecuritySide::PublicKeyLengthBytes, message.Data()[0].Data(), SecuritySide::PublicKeyLengthBytes);

				// Generate our own public key
				securitySide.GenerateKeyPair();

				Logger::Get()->LogThis("Server: Sending public key in response");

				// Send it to the server
				InternalSend(listeningSocket, reinterpret_cast<char*>(securitySide.GetPublicKey()),
					SecuritySide::PublicKeyLengthBytes, 0, reinterpret_cast<sockaddr*>(&fromClient.Address),
					fromClient.Length, deltaMs, SendPubKey);

				// We can derive the session keys
				securitySide.GenerateServerTransmissionKeys(remotePublicKey);

				sessionEstablished = true;
			}
		}
		else
		{			
			Networking::Get()->netError(bytesReceived, WSAGetLastError(), "Error listening for player traffic");
		}
	}

	int ReliableUdpGameServerConnection::InternalSend(const SOCKET socket, const char* buf, int len, const int flags,  const sockaddr* to, const int toLen, const unsigned long sendTimeMs, MessageType messageType)
	{
		BitPacker packer(readBuffer, ReadBufferMaxElements);
				
		// Prepare data to be sent
		const auto data = PacketDatum(false, buf, sendTimeMs);		

		// Add data to message and mark message as having been sent (we sent it later)
		const auto message = reliableUdp.MarkSent(data, messageType);

		// Write message to network buffer		
		message->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));

		if(sessionEstablished  && useEncryption)
		{
			// Use a constant nonce for now
			*sharedNonce = 1;

			// We will encrypt the message with extra bytes for auth tag
			std::vector<unsigned char> encryptedMessage(packer.TotalBytesPacked() + crypto_secretbox_MACBYTES);

			// Encrypt message
			Security::EncryptWithSessionKey(reinterpret_cast<const unsigned char*>(readBuffer), packer.TotalBytesPacked(),
			                                securitySide.GetTransmissionSessionKey(), sharedNonce,
			                                encryptedMessage.data());

			// send Encrypted message
			return sendto(socket, reinterpret_cast<char*>(encryptedMessage.data()), static_cast<int>(encryptedMessage.size()), flags, to, toLen);
		}

		// Send network buffer over udp		
		return sendto(socket, reinterpret_cast<char*>(readBuffer), countBytesToSend, flags, to, toLen);
	}
	
	int ReliableUdpGameServerConnection::InternalSend(const SOCKET socket, const char* buf, int len, const int flags,  const sockaddr* to, const int toLen, const unsigned long sendTimeMs)
	{		
		return InternalSend(socket, buf, len, flags, to, toLen, sendTimeMs, General);
	}

	int ReliableUdpGameServerConnection::SendAck(const SOCKET socket, const Message& messageToAck, const int flags, PeerInfo& peerInfo, const unsigned long sendTimeMs)
	{
		std::stringstream ackMessage;

		ackMessage << "server acks client seq:" << messageToAck.Header.Sequence << '\n';		

		Logger::Get()->LogThis("Server: Sending ack.");

		return InternalSend(socket, ackMessage.str().c_str(), static_cast<int>(ackMessage.str().size()), flags,
		                    reinterpret_cast<sockaddr*>(&peerInfo.Address), peerInfo.Length, sendTimeMs, Ack);
	}
}



