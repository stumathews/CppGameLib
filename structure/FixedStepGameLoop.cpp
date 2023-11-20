#include "FixedStepGameLoop.h"
#include <windows.h>

#include "objects/GameWorldData.h"
#include "structure/Profiler.h"

gamelib::FixedStepGameLoop::FixedStepGameLoop(
	const unsigned int timeStepMs,
	std::function<void(unsigned long deltaMs)> updateFunc,
	std::function<void()> drawFunc,
	std::function<void(unsigned long deltaMs)> inputFunc)
	: UpdateFunc(std::move(updateFunc)), DrawFunc(std::move(drawFunc)), InputFunc(std::move(inputFunc)), TimeStepMs(timeStepMs)
{
}

inline long gamelib::FixedStepGameLoop::GetTimeNowMs() { return static_cast<long>(timeGetTime()); }

inline void gamelib::FixedStepGameLoop::Update(const unsigned long deltaMs)
{
	UpdateFunc(deltaMs);
}

inline void gamelib::FixedStepGameLoop::Draw() { DrawFunc(); }

void gamelib::FixedStepGameLoop::Loop(const GameWorldData* gameWorldData)
{
	// fixed loop strategy: https://gameprogrammingpatterns.com/game-loop.html
	double previous = GetTimeNowMs();
	double lag = 0.0;

	while (!gameWorldData->IsGameDone)
	{
		const double current = GetTimeNowMs();
		const double elapsed = current - previous;
		previous = current;
		lag += elapsed;

		InputFunc(TimeStepMs);

		while (lag >= TimeStepMs)
		{
			Update(TimeStepMs);
			lag -= TimeStepMs;
		}

		Draw();
	}
}
