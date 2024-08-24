#include "ReliableUdpProtocolManager.h"

#include "PacketDatumUtils.h"
#include "PeerInfo.h"
#include "events/EventFactory.h"
#include "file/SerializationManager.h"
#include "net/Message.h"

namespace gamelib
{

	ReliableUdpProtocolManager::ReliableUdpProtocolManager(std::shared_ptr<IConnectedNetworkSocket> gameClientConnection, bool useEncryption)
	: gameClientConnection(std::move(gameClientConnection)), useEncryption(useEncryption)
{
		ReliableUdpProtocolManager::Initialize();
	}

	ReliableUdpProtocolManager::ReliableUdpProtocolManager(std::shared_ptr<IGameServerConnection> gameServerConnection, bool useEncryption)
	: gameServerConnection(std::move(gameServerConnection)), useEncryption(useEncryption)
{
	}

	bool ReliableUdpProtocolManager::Initialize()
	{
		clientSecuritySide.GenerateKeyPair();
				
		return true;
	}

	void ReliableUdpProtocolManager::Connect(const char* address, const char* port)
	{
		gameClientConnection->Connect(address, port);

		if(!sessionEstablished)
		{
			// Send our public key to server. Server should respond with theirs.
			SendInternal(reinterpret_cast<char*>(clientSecuritySide.GetPublicKey()), SecuritySide::PublicKeyLengthBytes, 0, SendPubKey);
		}
	}

	int ReliableUdpProtocolManager::Send(const char* callersSendBuffer, const int dataLength, const unsigned long deltaMs)
	{
		if(!sessionEstablished)
		{
			// Do not send the data if the session is not encrypted.

			// Send our public key to server instead.
			return SendInternal(reinterpret_cast<char*>(clientSecuritySide.GetPublicKey()), SecuritySide::PublicKeyLengthBytes, deltaMs, SendPubKey);;
		}

		// Send over the transport - might be a aggregated message if there were no prior acks		
		return SendInternal(callersSendBuffer, dataLength, deltaMs, General);
	}	

	int ReliableUdpProtocolManager::SendInternal(const char* callersSendBuffer, const int dataLength, const unsigned long deltaMs, const MessageType messageType)
	{
		BitPacker packer(packingBuffer, PackingBufferElements);

		// Track/store message as sent
		const auto message = reliableUdp.MarkSent(PacketDatum(false, callersSendBuffer, deltaMs), messageType);

		// Packet loss detected as we have unacknowledged data in send buffer that we are resending
		if(message->DataCount() > 1)
		{		
			this->RaiseEvent(EventFactory::Get()->CreateReliableUdpPacketLossDetectedEvent(std::make_shared<Message>(*message)));
		}

		// Write to the packingBuffer via the BitPacker	
		message->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		if(sessionEstablished && useEncryption)
		{
			// Use a constant nonce for now
			*sharedNonce = 1;

			// We will encrypt the message with extra bytes for auth tag
			std::vector<unsigned char> encryptedMessage(packer.TotalBytesPacked() + crypto_secretbox_MACBYTES);

			// Encrypt message
			Security::EncryptWithSessionKey(reinterpret_cast<const unsigned char*>(packingBuffer), packer.TotalBytesPacked(),
			                                clientSecuritySide.GetTransmissionSessionKey(), sharedNonce,
			                                encryptedMessage.data());

			// send Encrypted message
			return gameClientConnection->Send(reinterpret_cast<char*>(encryptedMessage.data()), encryptedMessage.size());
		}
		
		const auto sendResult = gameClientConnection->Send(reinterpret_cast<char*>(packingBuffer), countBytesToSend);
		return sendResult;
	}

	int ReliableUdpProtocolManager::ReceiveInternal(char* callersReceiveBuffer, const int bufLength, const unsigned long deltaMs)
	{
		// Read the payload off the network, wait for all the data
		const int bytesReceived = gameClientConnection->Receive(callersReceiveBuffer, bufLength);

		if(bytesReceived < 0)
		{
			return bytesReceived;
		}

		// Hook up to a 32-bit bitfield reader to the received buffer. This will read 32-bits at a time
		const auto count32BitBlocks = bytesReceived * 8 /32;
				
		if(sessionEstablished && useEncryption)
		{
			// We will use the same nonce for now (constant)
			*sharedNonce = 1;

			// We'll store the decrypted message minus the auth tag here
			std::vector<unsigned char> decryptedMessage(bytesReceived - crypto_secretbox_MACBYTES);

			// decrypt message
			auto decrptResult = Security::Get()->DecryptWithSessionKey(
				reinterpret_cast<const unsigned char*>(callersReceiveBuffer),
				bytesReceived,
				clientSecuritySide.GetReceiveSessionKey(), 
				sharedNonce,
				decryptedMessage.data());

			if(decrptResult < 0)
			{
				return bufLength;
			}
			// copy decrypted message into caller's buffer
			memcpy_s(callersReceiveBuffer, bufLength, decryptedMessage.data(), decryptedMessage.size());
		}

		
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
			RaiseEvent(EventFactory::Get()->CreateReliableUdpAckPacketEvent(std::make_shared<Message>(message), true));
		} 		
		
		if (message.Header.MessageType == 0) // ack
		{			
			RaiseEvent(EventFactory::Get()->CreateReliableUdpAckPacketEvent(std::make_shared<Message>(message), false));
			RaiseEvent(EventFactory::Get()->CreateReliableUdpPacketRttCalculatedEvent(std::make_shared<Message>(message), 
					PacketDatumUtils::CalculateRttStats(message, reliableUdp.SendBuffer)));
		}
		
		if(message.Header.MessageType == SendPubKey)
		{
			// Server sent us their public key
			memcpy_s(remotePublicKey, SecuritySide::PublicKeyLengthBytes, message.Data()[0].Data(), SecuritySide::PublicKeyLengthBytes);

			// We Can establish the session keys now
			clientSecuritySide.GenerateClientTransmissionKeys(remotePublicKey);

			// Prevent re-sending our public key
			sessionEstablished = true;
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

	int ReliableUdpProtocolManager::Receive(char* callersReceiveBuffer, const int bufLength, const unsigned long deltaMs)
	{

		const auto bytesReceived = ReceiveInternal(callersReceiveBuffer, bufLength, deltaMs);
		

		return bytesReceived;
	}

	int ReliableUdpProtocolManager::SendAck(const Message& messageToAck, const unsigned long sendTimeMs)
	{		
		BitPacker packer(packingBuffer, PackingBufferElements);

		std::stringstream ackMessageContents;
			ackMessageContents << "client acks server's seq:" << messageToAck.Header.Sequence << std::endl;

		// Prepare data to be sent - not ack will be pre-acked so we don't try to resend it
		const auto data = PacketDatum(true, ackMessageContents.str().c_str(), sendTimeMs);		

		// Add data to message and mark message as having been sent (we sent it later)
		auto* ackMessage = reliableUdp.MarkSent(data, Ack);

		// Write message to network buffer		
		ackMessage->Write(packer);

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		// Send network buffer over udp
		return gameClientConnection->Send(reinterpret_cast<char*>(readBuffer), countBytesToSend);
	}

	void ReliableUdpProtocolManager::Update(unsigned long deltaMs)
	{
	}

	std::shared_ptr<IConnectedNetworkSocket> ReliableUdpProtocolManager::GetConnection()
	{
		return gameClientConnection;
	}

	std::string ReliableUdpProtocolManager::GetSubscriberName()
	{
		return "ReliableUdpProtocolManager";
	}

	std::vector<std::shared_ptr<Event>> ReliableUdpProtocolManager::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{
		return {};
	}

	ReliableUdpProtocolManager::~ReliableUdpProtocolManager() = default;

	int ReliableUdpProtocolManager::GetSubscriberId()
	{
		return EventSubscriber::GetSubscriberId();
	}	
}
