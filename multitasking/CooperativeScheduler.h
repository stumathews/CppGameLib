#ifndef COOPERATIVE_SCHEDULER_H
#define COOPERATIVE_SCHEDULER_H
#include <vector>
#include <functional>
#include <algorithm>
#include <chrono>
#include <multitasking/Task.h>

namespace gamelib 
{
    class CooperativeScheduler 
    {
    public:

		// Schedules a new task with the given step function, priority, time slice, and deadline.
        void Add(Task::StepFn fn, int priority = 0, int timeSlice = 1, uint64_t deadline = UINT64_MAX) 
        {
            tasks.push_back(Task{ fn, priority, timeSlice, deadline });
        }

        // This gives the scheduler the opportunity to run all its tasks, one by one,
        // 
        void Update(uint64_t now) 
        {
            // Remove expired tasks
            tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
                [&](const Task& t) { return now > t.deadline; }),
                tasks.end());

            // Sort by priority (descending)
            std::sort(tasks.begin(), tasks.end(),
                [](const Task& a, const Task& b) { return a.priority > b.priority; });

            // Run tasks cooperatively
            for (size_t i = 0; i < tasks.size(); ) 
            {
                bool stillRunning = true;

				// A task can run for a maximum of timeSlice steps in one update cycle
                for (int step = 0; step < tasks[i].timeSlice && stillRunning; ++step)
                    stillRunning = tasks[i].stepFn();

				// if the task is no longer running (stepFn returned false), remove it from the list
                if (!stillRunning) 
                {
                    tasks.erase(tasks.begin() + i);
                }
                else 
                {
					// Move to the next task
                    ++i;
                }
            }
        }

    private:

		// List of scheduled tasks
        std::vector<Task> tasks;
    };
}

#endif