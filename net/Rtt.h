#pragma once
#ifndef RTT_H
#define RTT_H

struct Rtt
{
	// round trip time
	unsigned long rtt;

	// smooth moving average, k=3 (considers the last 3 rtts)
	float Sma3;
};

#endif