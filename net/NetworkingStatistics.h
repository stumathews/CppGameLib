#pragma once
#ifndef NETWORKING_STATISTICS_H
#define NETWORKING_STATISTICS_H

namespace gamelib
{
	struct NetworkingStatistics
	{
		int CountPacketsLost {0};
		float AverageLatency {0.0};
		int BandwidthUsed {0};
		int VerificationFailedCount {0};
		int CountAcks;
		int CountPacketsReceived {0};

		void Reset()
		{
			CountPacketsLost =  BandwidthUsed = VerificationFailedCount = CountAcks = CountPacketsReceived = 0;
			AverageLatency = 0.0;
		}
	};
}

#endif