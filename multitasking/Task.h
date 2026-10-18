#ifndef TASK_H
#define TASK_H

#include <functional>
#include <chrono>

namespace gamelib {

	// A task is a lightweight unit of yielding work that can be scheduled and executed by the CooperativeScheduler. 
    // Each task has a step function, a priority, a time slice, and a deadline.
	// The work should be broken into small steps, and the step function should 
    // return true if there is more work to do, or false if the task is complete.
    struct Task 
    {
        using StepFn = std::function<bool()>; // true = still running

		StepFn stepFn;       // function to call for each step
        int priority;        // higher = run earlier
        int timeSlice;       // max steps per update
        uint64_t deadline;   // timestamp or frame number
    };
}

#endif

