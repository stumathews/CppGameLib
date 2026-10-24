#include <future>

#include "pch.h"
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>
#include <gtest/gtest.h>

#include "buffering/DoubleBuffer.h"
#include "multitasking/BackgroundWorker.h"


using namespace std;

namespace gamelib
{
	class BackgroundWorkerTests : public testing::Test
	{
	protected:

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}
	};

	TEST_F(BackgroundWorkerTests, SimpleUsage)
	{
		// Starts a background worker.
		BackgroundWorker& background = BackgroundWorker::instance();

		struct JobResult
		{
			int Value = 0;
			bool IsDone = false;
		};

		DoubleBuffer<JobResult> sharedBuffer;

		// A promise to signal when the job is done, and a future to wait for it
		std::promise<void> promise;
		const std::future<void> future = promise.get_future();

		// Submits a job asynchronously to it
		background.Run({ 
			.func = [buffer = &sharedBuffer, &promise]() mutable{
				
				{
					// this expensive operation will run on the background thread
				}
				// We'll write the result back to the buffer
				buffer->GetWriteBuffer() = { .Value = 42, .IsDone = true };
				
				// We'll publish the result so that the main thread can read it
				buffer->Publish();

				// Signal that the job is done
				promise.set_value();
		} });

		// Wait on the future, which will block until the promise is set in the background thread
		future.wait();

		// Now we can read the result from the buffer
		const auto readBuffer = sharedBuffer.GetReadBuffer();
		const auto readResult = *readBuffer;

		// Check that the result is what we expect
		EXPECT_THAT(readResult.IsDone, testing::IsTrue());
		EXPECT_THAT(readResult.Value, testing::Eq(42));

	}
}