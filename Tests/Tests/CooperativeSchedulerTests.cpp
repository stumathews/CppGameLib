#include "pch.h"
#include <gtest/gtest.h>
#include "multitasking/CooperativeScheduler.h"
#include "multitasking/Task.h"

namespace gamelib
{
	class CooperativeSchedulerTests : public testing::Test
	{
	public:

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};

	TEST_F(CooperativeSchedulerTests, SimpleUsage)
	{
		// Will give each of our tasks time to run
		CooperativeScheduler scheduler;

		// We'll track the order of execution of tasks to test priority
		std::vector<int> orderOrExecution;

		// A task result 
		struct TaskResult
		{
			bool didTaskRun;
			int slicesGiven;
		};

		// Prepare task results - tasks will update these
		TaskResult task1Result = { false, 0};
		TaskResult task2Result = { false, 0};
		TaskResult task3Result = { false, 0};
		TaskResult task4Result = { false, 0};

		// We will have 4 tasks:

		Task::StepFn task1 = [&]
		{
			// Record order of execution
			if (!task1Result.didTaskRun)
				orderOrExecution.push_back(1);

			// Record that the task ran and how many slices it was given
			task1Result.didTaskRun = true;
			task1Result.slicesGiven++;

			return true;
		};

		Task::StepFn task2 = [&]
		{
			// Record order of execution
			if (!task2Result.didTaskRun)
				orderOrExecution.push_back(2);

			// Record that the task ran and how many slices it was given
			task2Result.didTaskRun = true;
			task2Result.slicesGiven++;

			return true;
		};

		Task::StepFn task3 = [&]
		{
			// Record order of execution
			if (!task3Result.didTaskRun)
				orderOrExecution.push_back(3);

			// Record that the task ran and how many slices it was given
			task3Result.didTaskRun = true;			
			task3Result.slicesGiven++;
			return true;
		};

		Task::StepFn task4 = [&]
		{
			// Record order of execution
			if (!task4Result.didTaskRun)
				orderOrExecution.push_back(4);

			// Record that the task ran and how many slices it was given
			task4Result.didTaskRun = true;
			task4Result.slicesGiven++;
			return true;
		};

		scheduler.Add(task1,10, 1, 10);
		scheduler.Add(task2, 20, 10, 10);
		scheduler.Add(task3, 100, 4, 10);
		scheduler.Add(task4, 100, 4, 4);

		// Task should not have run yet
		ASSERT_FALSE(task1Result.didTaskRun);
		ASSERT_FALSE(task2Result.didTaskRun);
		ASSERT_FALSE(task3Result.didTaskRun);
		ASSERT_FALSE(task4Result.didTaskRun);

		// Run all tasks (those that have a deadline > 12 are removed)
		scheduler.Update(5);

		// Task should have run
		ASSERT_TRUE(task1Result.didTaskRun);
		ASSERT_TRUE(task2Result.didTaskRun);
		ASSERT_TRUE(task3Result.didTaskRun);
		ASSERT_FALSE(task4Result.didTaskRun); // missed deadline

		ASSERT_EQ(task1Result.slicesGiven, 1);
		ASSERT_EQ(task2Result.slicesGiven, 10);
		ASSERT_EQ(task3Result.slicesGiven, 4);
		ASSERT_EQ(task4Result.slicesGiven, 0);

		ASSERT_EQ(orderOrExecution[0], 3); // task 3 has highest priority
		ASSERT_EQ(orderOrExecution[1], 2);
		ASSERT_EQ(orderOrExecution[2], 1); // task 1 has lowest priority

	}
};