#pragma once
#ifndef NETWORKING_STATISTICS_H
#define NETWORKING_STATISTICS_H

namespace gamelib
{
	struct NetworkingStatistics
	{
		int CountPacketsLost {0};
		float AverageLatency {0.0}; // SMA3
		int BytesReceived {0};
		int VerificationFailedCount {0};
		int CountAcks {0};
		int CountPacketsReceived {0};
		int CountAggregateMessagesReceived {0};
		int SendRateMs {0};
		int SendRatePs {0}; // sends per second

		void Reset()
		{
			CountPacketsLost =  BytesReceived = VerificationFailedCount = CountAcks = CountPacketsReceived = CountAggregateMessagesReceived = 0;
			AverageLatency = 0.0;
		}
	};
}

#endif