
#include "pch.h"
#include "TestData.h"
#include <utils/BitFiddler.h>
#include <net/BitPacker.h>

#include "events/NetworkTrafficReceivedEvent.h"
#include "gtest/gtest.h"
#include "net/BitPackingTypes.h"
#include "net/Fish.h"
#include "net/PingMessage.h"

namespace gamelib
{
	class SerializationTests : public testing::Test
	{
	public:	

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};

	TEST_F(SerializationTests, PingPong)
	{
		PingMessage pingMessage;
		pingMessage.Type = "ping";
		pingMessage.IsHappy = true;
		pingMessage.EventType = NetworkTrafficReceivedEventId.PrimaryId;
		pingMessage.Names = { "Stuart", "Jenny", "bruce" };
		pingMessage.Ages = {1, 2,3 };
		pingMessage.Fish = Fish("Neemo", "Mathews");

		uint32_t networkBuffer[20] = {0}; // 80 byte network buffer

		// Pack
		BitPacker bitPacker(networkBuffer, 20); // 32-bit packer

		pingMessage.Write(bitPacker);
		EXPECT_EQ(bitPacker.TotalBitsPacked(), 576); // 72 Bytes 

		// Unpack
		BitfieldReader bitFieldReader(networkBuffer, bitPacker.TotalBytesPacked());
		//BitfieldReader bitFieldReader(networkBuffer, 20);
		PingMessage deserializedPingMessage;
		deserializedPingMessage.Read(bitFieldReader);

		EXPECT_STREQ(pingMessage.Type.c_str(), deserializedPingMessage.Type.c_str());
		EXPECT_EQ(pingMessage.IsHappy, deserializedPingMessage.IsHappy);
		EXPECT_EQ(pingMessage.EventType, deserializedPingMessage.EventType);
		EXPECT_EQ(pingMessage.Names.size(), deserializedPingMessage.Names.size());
		for(int i = 0; i < pingMessage.Names.size();i++)
		{
			EXPECT_STREQ(pingMessage.Names[i].c_str(), deserializedPingMessage.Names[i].c_str());
		}

		for(int i = 0; i < pingMessage.Ages.size();i++)
		{
			EXPECT_EQ(pingMessage.Ages[i], deserializedPingMessage.Ages[i]);
		}

		EXPECT_STREQ(pingMessage.Fish.Name.c_str(), deserializedPingMessage.Fish.Name.c_str());
		EXPECT_STREQ(pingMessage.Fish.Surname.c_str(), deserializedPingMessage.Fish.Surname.c_str());

	}
}
