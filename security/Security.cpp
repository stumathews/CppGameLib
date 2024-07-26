#include "Security.h"

namespace gamelib
{
	Security* Security::Get()
	{
		if (instance == nullptr)
		{
			instance = new Security();
			instance->Initialize();
		}
		return instance;
	}

	Security::Security()
	{
		Initialize();
	}

	Security* Security::instance = nullptr;

	
	Security::~Security()
	{
		
		instance = nullptr;
	}


	int Security::EncryptWithSessionKey(const unsigned char* message, const unsigned long long messageLengthBytes,
	                                    const unsigned char* sessionKey, const unsigned char* nonce,
	                                    /* Output */ unsigned char* encryptedMessageWithAuthTag)
	{
		// With a nonce we never used the same one with the symmetric key, it must vary while the symmetric key remains by definition constant

		// encrypt and prepend a crypto_secretbox_MACBYTES auth tag to encrypted message (integrity+authentication)
		return crypto_secretbox_easy(encryptedMessageWithAuthTag, message, messageLengthBytes, nonce, sessionKey);
	}

	int Security::DecryptWithSessionKey(const unsigned char* encryptedMessageWithAuthTag,
	                                    const unsigned long long encryptedMessageWithAuthTagLengthBytes,
	                                    const unsigned char* sessionKey, const unsigned char* nonce,
	                                    /* Output */ unsigned char* decryptedMessage)
	{
		// verify and decrypt 
		return crypto_secretbox_open_easy(decryptedMessage, encryptedMessageWithAuthTag, encryptedMessageWithAuthTagLengthBytes, nonce, sessionKey);
	}

	bool Security::Initialize()
	{
		if (sodium_init() < 0) 
		{
			initialized = false;
	    }
	    initialized = true;

		return initialized;
	}
}
