#ifndef RELIABLE_UDP_CHECKSUM_FAILED_EVENT_H
#pragma once
#include "Event.h"
#include "EventNumbers.h"

namespace gamelib
{
	class Message;
	const static EventId ReliableUdpCheckSumFailedEventId(ReliableUdpCheckSumFailed, "ReliableUdpCheckSumFailed");
	class ReliableUdpCheckSumFailedEvent : public Event
	{
	public:
		explicit ReliableUdpCheckSumFailedEvent(std::shared_ptr<Message> failedMessage)
			: Event(ReliableUdpCheckSumFailedEventId), failedMessage(std::move(failedMessage))
		{
		}

		std::shared_ptr<Message> failedMessage;
	};
}

#endif RELIABLE_UDP_CHECKSUM_FAILED_EVENT_H