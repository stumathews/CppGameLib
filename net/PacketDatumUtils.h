#pragma once
#ifndef PACKET_DATUM_UTILS
#define PACKET_DATUM_UTILS
#include <vector>

#include "PacketDatum.h"
#include "utils/RingBuffer.h"

namespace gamelib
{
	struct PacketDatumUtils
	{
		static std::vector<unsigned long> GetLastKRtts(const int k, RingBuffer<PacketDatum>& receiveBuffer)
		{
			std::vector<unsigned long> p {0}; // dataPoints
			const auto n = receiveBuffer.GetLastAddedSequence(); // number of dataPoints
			for(int i = 1 ; i <= k ; i++)
			{
				const auto* datum = receiveBuffer.Get(n-k+i);
				if( datum == nullptr)
				{
					p.push_back(0);
				}
				else
				{
					p.push_back(datum->RttMs);
				}
			}
			return p;
		}
	};

};

#endif