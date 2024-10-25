#include "pch.h"
#include "security/Security.h"
#include <limits>
#include <gtest/gtest.h>

#pragma comment(lib, "LIBSodium.lib")

namespace gamelib
{
	class SecurityTests : public testing::Test
	{
	public:	

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};


	TEST_F(SecurityTests, KeyExchangeAndEncryptDescrypt)
	{
		SecuritySide client;
		SecuritySide server;

		client.GenerateKeyPair();	
		server.GenerateKeyPair();

		// .. Share public keys between client and server

		// Generate same keys like diffie hellman does for perfect forward security
		client.GenerateClientTransmissionKeys(server.GetPublicKey());
		server.GenerateServerTransmissionKeys(client.GetPublicKey());

		// client and server have generated the same 2 shared keys (transmissionSessionKey and ReceiveSessionKey)

		// Make a nonce which should be the same when encrypting and decrypting on both sides but should be thrown away after each time encryption/decryption attempt
		unsigned char sharedNonce[SecuritySide::NonceLengthBytes]{0};
		randombytes_buf(sharedNonce, SecuritySide::NonceLengthBytes);

		// Plain text
		constexpr unsigned char message[18] = "My name is Stuart";
		unsigned char decryptedMessage[18] {0};

		// Where the encrypted message will be written to
		std::vector<unsigned char> encryptedMessage(18 + crypto_secretbox_MACBYTES);

		// Encrypt & transmit with key1
		Security::Get()->EncryptWithSessionKey(message, 18, client.GetTransmissionSessionKey(), sharedNonce,
		                                                     encryptedMessage.data());

		// Receive & decrypt with key1
		Security::Get()->DecryptWithSessionKey(encryptedMessage.data(), 18 + crypto_secretbox_MACBYTES,
		                                       server.GetReceiveSessionKey(), sharedNonce, decryptedMessage);
		for(auto i = 0; i < 18;i++ )
		{
			EXPECT_EQ(message[i], decryptedMessage[i]);
		}
	}

	TEST_F(SecurityTests, TamperTest)
	{
		SecuritySide client;
		SecuritySide server;

		client.GenerateKeyPair();	
		server.GenerateKeyPair();

		// .. Share public keys between client and server

		// Generate same keys like diffie hellman does for perfect forward security
		client.GenerateClientTransmissionKeys(server.GetPublicKey());
		server.GenerateServerTransmissionKeys(client.GetPublicKey());

		// client and server have generated the same 2 shared keys (transmissionSessionKey and ReceiveSessionKey)

		// Make a nonce which should be the same when encrypting and decrypting on both sides but should be thrown away after each time encryption/decryption attempt
		unsigned char sharedNonce[SecuritySide::NonceLengthBytes]{0};
		randombytes_buf(sharedNonce, SecuritySide::NonceLengthBytes);

		// Plain text
		constexpr unsigned char message[18] = "My name is Stuart";
		unsigned char decryptedMessage[18] {0};

		// Where the encrypted message will be written to
		std::vector<unsigned char> encryptedMessage(18 + crypto_secretbox_MACBYTES);

		// Encrypt & transmit with key1
		Security::Get()->EncryptWithSessionKey(message, 18, client.GetTransmissionSessionKey(), sharedNonce,
		                                       encryptedMessage.data());

		// Manipulate the encrypted message, simulating tampering
		encryptedMessage[2] = 's'; // arbitrary change

		// Receive & decrypt with key1
		const auto decryptResult = Security::Get()->DecryptWithSessionKey(encryptedMessage.data(), 18 + crypto_secretbox_MACBYTES,
		                                                                  server.GetReceiveSessionKey(), sharedNonce, decryptedMessage);
		// Expect the decrypt to have failed
		EXPECT_EQ(decryptResult, -1);

		for(auto i = 0; i < 18;i++ )
		{
			if(i == 17) 
				EXPECT_EQ(message[i], decryptedMessage[i]); // we expect the null terminator to be the same on both
			else
				EXPECT_NE(message[i], decryptedMessage[i]); // but the rest of the values to be non-identical
		}
	}
}