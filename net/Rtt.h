#pragma once
#ifndef RTT_H
#define RTT_H

namespace gamelib
{
	
	struct Rtt
	{
		Rtt(unsigned long rtt,float sma3 = 0.0f):rtt(rtt),Sma3(sma3){};
		// round trip time
		unsigned long rtt;

		// smooth moving average, k=3 (considers the last 3 rtts)
		float Sma3;
	};
}

#endif
