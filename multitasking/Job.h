#ifndef JOB_H
#define JOB_H

#include <functional>

namespace gamelib
{
	
	struct Job 
	{
		// A job is a unit of work that can be executed by a background worker. 
		// It contains a function that takes a DoubleBuffer<T> as an argument.
		std::function<void()> func;
	};
}

#endif