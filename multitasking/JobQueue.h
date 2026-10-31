#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include "Job.h"

namespace gamelib
{
	// A thread-safe queue for managing jobs to be executed by background workers.
    class JobQueue 
    {
    public:
        void Push(Job job) 
        {
			// Start local scope for lock_guard to ensure the mutex is released after pushing the job
            {
				// Take a lock to ensure thread safety when accessing the queue
				std::scoped_lock lock(mutex);

				// put the job into the queue
                jobQueue.push(std::move(job));

            } // lock is released here
            
			// Notify one to wake up a thread that is waiting on the condition variable, as a new job is available
            cv.notify_one();
        }

        Job pop() 
        {
			// Only one thread can pop at a time, so we use a unique_lock to
			// allow waiting on the condition variable
            std::unique_lock lock(mutex);

			// If predicate is not satisfied,
			cv.wait(lock, // unlock the mutex that was acquired (above) and sleep the thread
				// until...it is satisfied
				[&] { return !jobQueue.empty() || stop_; }); // on wake (even if unexpected), now re-acquire the lock and check the predicate again (sleep if still unsatisfied)

            if (stop_) 
            { 
                // return invalid job
                return Job 
                { 
                	.func = [] {}
                };             
            }

			// Get the job from the front of the queue and remove it
            Job job = std::move(jobQueue.front());
            jobQueue.pop();

			// Return the job to the caller
            return job;
        }

        void stop() 
        {
            {
				// Take a lock to ensure thread safety when setting the stop flag
                std::scoped_lock lock(mutex);
                stop_ = true;
            }

			// Notify all waiting threads that the queue is stopping
            cv.notify_all();
        }

    private:

		// The queue that holds the jobs to be processed
        std::queue<Job> jobQueue;

		// Mutex to protect access to the queue and the stop flag
        std::mutex mutex;

		// Condition variable to notify waiting threads when a new job is available or when stopping
        std::condition_variable cv;

		// Flag to indicate that the queue is stopping and no more jobs should be processed
        bool stop_ = false;
    };
}

#endif
