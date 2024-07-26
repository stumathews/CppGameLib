#ifndef SECURITY_H
#define SECURITY_H
#pragma once
#include "sodium.h"

namespace gamelib
{


	class Security
	{
		protected:
			static Security* instance;
			bool initialized{};		
			Security();

		public:		
			static Security* Get();

			// Cannot copy
			Security(Security const&) = delete;

			// Cannot assign to
			void operator=(Security const&) = delete;
		
			~Security();

			static int EncryptWithSessionKey(const unsigned char* message, const unsigned long long messageLengthBytes,
			                                 const unsigned char* sessionKey, const unsigned char* nonce,
			                                 unsigned char* encryptedMessageWithAuthTag);
			static int DecryptWithSessionKey(const unsigned char* encryptedMessageWithAuthTag, const unsigned long long encryptedMessageWithAuthTagLengthBytes, const
			                                 unsigned char* sessionKey, const unsigned char* nonce, unsigned char* decryptedMessage);
			bool Initialize();	
	};

	
	class SecuritySide
	{
		unsigned char publicKey[crypto_kx_PUBLICKEYBYTES] {0};
		unsigned char privateKey[crypto_kx_SECRETKEYBYTES] {0};

		// These keys are the same for the server and the client
		unsigned char transmitSessionKey[crypto_kx_SESSIONKEYBYTES] {0}; // key used to send to remote
		unsigned char receiveSessionKey[crypto_kx_SESSIONKEYBYTES] {0};  // key used to receive data from remote

		unsigned char nonceBuf[crypto_secretbox_NONCEBYTES] {0};	

	public:
		SecuritySide()
		{
			Security::Get()->Initialize(); // this is allowed to be called multiple times
		}
		void GenerateKeyPair()
		{
			crypto_kx_keypair(publicKey, privateKey);
		}

		int GenerateClientTransmissionKeys(unsigned char serverPublicKey[32])
		{		
			return crypto_kx_client_session_keys(receiveSessionKey,transmitSessionKey, publicKey, privateKey, serverPublicKey);
		}

		int GenerateServerTransmissionKeys(unsigned char clientPublicKey[32])
		{
			return crypto_kx_server_session_keys(receiveSessionKey, transmitSessionKey, publicKey, privateKey, clientPublicKey);
		}

		unsigned char* GenerateNonce()
		{
			randombytes_buf(nonceBuf, NonceLengthBytes);
			return nonceBuf;
		}

		unsigned char* GetPublicKey() { return publicKey; }
		unsigned char* GetPrivateKey() { return privateKey; }

		unsigned char* GetTransmissionSessionKey () { return transmitSessionKey; }
		unsigned char* GetReceiveSessionKey() { return receiveSessionKey; }

		static constexpr unsigned int PublicKeyLengthBytes = crypto_kx_PUBLICKEYBYTES;
		static constexpr unsigned int PrivateKeyLengthBytes = crypto_kx_SECRETKEYBYTES;

		static constexpr unsigned int TransmissionSessionKeyLengthBytes = crypto_kx_SESSIONKEYBYTES;
		static constexpr unsigned int ReceiveSessionKeyLengthBytes = crypto_kx_SESSIONKEYBYTES;

		static constexpr unsigned int NonceLengthBytes = crypto_secretbox_NONCEBYTES;
	};

	
}
#endif

