#ifndef RELIABLE_UDP_PACKET_RECEIVED_EVENT_H
#define RELIABLE_UDP_PACKET_RECEIVED_EVENT_H
#pragma once
#include "events/Event.h"
#include "events/EventNumbers.h"

namespace gamelib
{
	class Message;

	const static EventId ReliableUdpPacketReceivedEventId(ReliableUdpPacketReceived, "ReliableUdpPacketReceived");
	class ReliableUdpPacketReceivedEvent final : public Event
	{
	public:

		explicit ReliableUdpPacketReceivedEvent(const std::shared_ptr<Message> message) : Event(ReliableUdpPacketReceivedEventId), ReceivedMessage(message)
		{
		}

		const std::shared_ptr<Message> ReceivedMessage;
	};

}

#endif RELIABLE_UDP_PACKET_RECEIVED_EVENT_H