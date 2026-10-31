#ifndef BACKGROUND_WORKER_H
#define BACKGROUND_WORKER_H

#include "Job.h"
#include "JobQueue.h"

#include <thread>

namespace gamelib
{
	// A background worker that runs jobs in a separate thread.
    class BackgroundWorker 
    {
    public:

        BackgroundWorker() 
        {
			// Start the thread that will process jobs in the background
            thread = std::thread([this]
            {
	            ProcessJobs();
            });
        }

        void Run(Job job)
        {
            jobQueue.Push(std::move(job));
        }

        // Singleton instance of BackgroundWorker
        static BackgroundWorker& instance()
        {
            static BackgroundWorker worker;   // thread-safe since C++11
            return worker;
        }

        ~BackgroundWorker() 
        {
			// Automatically stop the worker when the instance is destroyed
            jobQueue.stop();

            running = false;

			// Let the background thread finish processing jobs and exit
            thread.join();
        }

    private:

		// Continuously processes jobs from the job queue in a separate thread.
        void ProcessJobs() 
        {            
            while (true) 
            {
				// Fetch job off jobQueue (or wait until job is available)
				Job job = jobQueue.pop(); 

            	if (!running) 
                {
					// Exit the loop if the worker is no longer running
                    break;
                }

				// Run the job's function
                job.func();
            }
        }

		// The job queue that holds the jobs to be executed by the background worker
        JobQueue jobQueue;
		
    	// The thread that runs the background worker
        std::thread thread;

		// Flag to indicate whether the background worker is running
        bool running = true;
    };

}

#endif
