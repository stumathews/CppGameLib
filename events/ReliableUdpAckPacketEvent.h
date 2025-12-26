#ifndef RELIABLE_ACK_PACKET_EVENT_H
#define RELIABLE_ACK_PACKET_EVENT_H
#pragma once
#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	class Message;
	const static EventId ReliableUdpAckPacketEventId(ReliableUdpAckPacket, "ReliableUdpAckPacketEvent");
	class ReliableUdpAckPacketEvent final : public Event
	{
	public:

		explicit ReliableUdpAckPacketEvent(const std::shared_ptr<Message> &message, const bool sent) : Event(ReliableUdpAckPacketEventId), ReceivedMessage(message), Sent(sent)
		{
		}

		const std::shared_ptr<Message> ReceivedMessage;
		bool Sent; // if not sent, its received
	};
}

#endif
