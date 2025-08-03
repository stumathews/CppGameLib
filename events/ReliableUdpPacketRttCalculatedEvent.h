#pragma once
#ifndef RELIABLE_UDP_PACKET_RTT_EVENT_H
#define RELIABLE_UDP_PACKET_RTT_EVENT_H
#include <memory>

#include "Event.h"
#include "EventId.h"
#include "EventNumbers.h"
#include "net/Rtt.h"

namespace gamelib
{
	class Message;
	const static EventId ReliableUdpPacketRttCalculatedEventId(ReliableUdpPacketRttCalculated, "ReliableUdpPacketRttCalculatedEvent");

	class ReliableUdpPacketRttCalculatedEvent final : public Event
	{
	public:

		explicit ReliableUdpPacketRttCalculatedEvent(std::shared_ptr<Message> message, const Rtt rtt)
		: Event(ReliableUdpPacketRttCalculatedEventId), ReceivedMessage(std::move(message)), Rtt(rtt)
		{
		}

		const std::shared_ptr<Message> ReceivedMessage;
		Rtt Rtt;
	};
}

#endif