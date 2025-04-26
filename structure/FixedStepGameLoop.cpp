#include <windows.h>
#include "FixedStepGameLoop.h"
#include "objects/GameWorldData.h"
#include "structure/Profiler.h"

gamelib::FixedStepGameLoop::FixedStepGameLoop(const unsigned int timeStepMs,
	std::function<void(unsigned long deltaMs)> updateFunc,
	std::function<void()> drawFunc,
	std::function<void(unsigned long deltaMs)> inputFunc)
	: UpdateFunc(std::move(updateFunc)),
      DrawFunc(std::move(drawFunc)),
      InputFunc(std::move(inputFunc)),
      TimeStepMs(timeStepMs) { }

void gamelib::FixedStepGameLoop::Loop(GameWorldData* gameWorldData)
{
	// fixed loop strategy: https://gameprogrammingpatterns.com/game-loop.html
	unsigned long previous = GetTimeNowMs();
	unsigned long lag = 0.0;

	while (!gameWorldData->IsGameDone)
	{
		const unsigned long current = GetTimeNowMs();
		const unsigned long elapsed = current - previous;

		gameWorldData->ElapsedGameTime += elapsed;

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

inline long gamelib::FixedStepGameLoop::GetTimeNowMs()
{
	return static_cast<long>(timeGetTime());
}

inline void gamelib::FixedStepGameLoop::Update(const unsigned long deltaMs)
{
	UpdateFunc(deltaMs);
}

inline void gamelib::FixedStepGameLoop::Draw()
{
	DrawFunc();
}