#include "VariableGameLoop.h"
#include <iostream>
#include <ostream>
#include "file/SettingsManager.h"
#include <windows.h>
#include "objects/GameWorldData.h"
#include "Profiler.h"


gamelib::VariableGameLoop::VariableGameLoop(std::function<void(unsigned long deltaMs)> updateFunc,
                                            std::function<void()> drawFunc)
	: updateFunc(std::move(updateFunc)), drawFunc(std::move(drawFunc)), gameWorldData(std::move(gameWorldData))
{
}

long GetTimeNowMs()
{
	return static_cast<long>(timeGetTime());
}

void gamelib::VariableGameLoop::Update(const unsigned long deltaMs)
{
	const auto duration = Profiler::GetDuration([&]()
	{
		updateFunc(deltaMs);
	});
	std::cout << "Update Cost: " << duration << "ms" << std::endl;
}

void gamelib::VariableGameLoop::Draw()
{
	drawFunc();
}

void gamelib::VariableGameLoop::Loop(GameWorldData* gameWorldData)
{
	const auto maxCatchupLoops = SettingsManager::Int("global", "max_loops");
		const auto printFrameRate = SettingsManager::Bool("gameStructure", "printFrameRate");

		// Tick/update every tick_time_ms, ie if tick_time_ms = 10, then will tick/update 10 times in one second
		const long TICK_TIME = SettingsManager::Int("global", "tick_time_ms");

		// A 'tick' reprents one update call(). 
		// We want a fixed number of ticks within a real unit of time. 
		// A real unit of time does not change (1 second is always one second in the real world)
		// irrespective of what the hardware you are running on. So if we can ensure we have the same number of updates in one second, the update rate
		// will always be te same, the we have a fixed update rate. 

		// t0 repreesnts the time when the last update() finished.
		// t1 when re-evaluated represents the time elapsed since t0 (last update)

		// t1-t0 is the elapsed time since the last update and is called the frame-time, and includes the time taken to draw.
		// The frame-time is the time it it takes to since the last update, and which includes subsequent drawing
		// that a new update might be ready to be called again or if not, more drawing can be done.

		auto elapsedTimeMs = 0;
		auto countUpdates = 0; // Number of loops

		// Initialize/prepare process by saying last update occured right now.
		auto t0 = GetTimeNowMs();

		while (!gameWorldData->IsGameDone) // main game loop (exact speed of loops is hardware dependent)
		{
			const auto t1 = GetTimeNowMs(); 

			// Update() can be called every tick_time_ms which will result in a constant amount of ticks, as a ms is a ms independent on hardware.			
			// Update() start at each 'tick' x times a second, depending on how long a tick is set to be, ie. tick_time_ms.
			// However: Update might run longer than a tick, so the next scheduled tick start time could be missed.
			auto loops = 0;
			while ((t1 - t0) >= TICK_TIME && loops <  maxCatchupLoops)
			{				
				Update(t1 - t0);
				
				t0 += TICK_TIME;
				loops++;

				elapsedTimeMs += TICK_TIME;
				countUpdates++;
				// if update took longer than tick_time_ms, the update which was supposed to happen didn't happen
				// and as a consequence the t0 is not up to date as the previous tick did not modify it.
				// As we finished after the next scheduled update() tick (and skipped that one)
				// we've only brought t0 up to the point where it should have been had the missed update been run, it hasn't
				// so we can try run update again in the while loop as many times as it takes to being t0 up to date.
				// this hopes that the these successive updates did not skip either and where shorter and so can make up for it.
				// the loops will keep being up t0 by a TICK_TIME until doing so means we can't loop anymore, but that also
				// means that we(t1) are < TICK_TIME (so its up to date now).
				// What this means is that the delta_time is back to a consistent value, even though some of the previous ticks did not happen
				// and would normally have made the delta_time differ due the variances caused by skipping/missing fixed schedules. Quite ingenious.

				// but I'm still getting variable time-slices. TODO: Fix this stuff with proper understanding
			}			
			
			if(printFrameRate && elapsedTimeMs > 1000)
			{
				std::cout << "Rate: " << countUpdates -1 << " Fps" << std::endl;
				elapsedTimeMs = 0;
				countUpdates = 0;
			}

			// at this point the stats we have are: 
			// 1) the elapsed time taken in 1 hardware loop
			// 2) the number of updates in 1 hardware loop, while (still ensuring we alway update every tick_time_ms)

			// Misc tasks

			//HandleSpareTime(elapsedTimeMs); // handle player input, general housekeeping (Event Manager processing)

			// drawing time!

			if (gameWorldData->IsNetworkGame && (t1 - t0) > TICK_TIME) { t0 = t1 - TICK_TIME; }

			if (gameWorldData->CanDraw)
			{
				// How much within the new 'tick' are we?
				const auto percentWithinTick = min(1.0f, (t1 - t0) / TICK_TIME);
				// NOLINT(bugprone-integer-division, clang-diagnostic-implicit-int-float-conversion)				
				//Draw(static_cast<unsigned int>(percentWithinTick));
				Draw();
			}
		}
}
