
#ifndef RELIABLE_UDP_PACKET_LOSS_DETECTED_EVENT_H
#define RELIABLE_UDP_PACKET_LOSS_DETECTED_EVENT_H

#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	class Message;
	const static EventId ReliableUdpPacketLossDetectedEventId(ReliableUdpPacketLossDetected, "ReliableUdpPacketLossDetected");
	class ReliableUdpPacketLossDetectedEvent : public Event
	{
	public:
		explicit ReliableUdpPacketLossDetectedEvent(std::shared_ptr<Message> messageBundle)
			: Event(ReliableUdpPacketLossDetectedEventId), messageBundle(std::move(messageBundle))
		{
		}

		std::shared_ptr<Message> messageBundle;
	};
}

#endif
