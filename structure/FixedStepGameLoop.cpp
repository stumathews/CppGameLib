#include "FixedStepGameLoop.h"
#include "objects/GameWorldData.h"
#include "structure/Profiler.h"
#include "time/DeltaTime.h"

gamelib::FixedStepGameLoop::FixedStepGameLoop(const unsigned int timeStepMs,
                                              std::function<void(const cppgamelib::DeltaTime* deltaTime)> updateFunc,
                                              std::function<void()> drawFunc,
                                              std::function<void(const cppgamelib::DeltaTime* deltaTime)> inputFunc)
	: UpdateFunc(std::move(updateFunc)),
      DrawFunc(std::move(drawFunc)),
      InputFunc(std::move(inputFunc)),
      TimeStepMs(timeStepMs) { }

void gamelib::FixedStepGameLoop::Loop(GameWorldData* gameWorldData)
{
	// fixed loop strategy: https://gameprogrammingpatterns.com/game-loop.html
	unsigned long previous = GetTimeNowMs();
	unsigned long lag = 0;
	cppgamelib::DeltaTime deltaTime;
	deltaTime.milliseconds = TimeStepMs;

	while (!gameWorldData->IsGameDone)
	{
		const unsigned long current = GetTimeNowMs();
		const unsigned long elapsed = current - previous;

		gameWorldData->ElapsedGameTime += elapsed;

		previous = current;
		lag += elapsed;

		InputFunc(&deltaTime);

		while (lag >= TimeStepMs)
		{
			Update(&deltaTime);
			lag -= TimeStepMs;
		}

		Draw();
	}
}

inline long gamelib::FixedStepGameLoop::GetTimeNowMs()
{
	return static_cast<long>(GetTimeMs());
}

inline void gamelib::FixedStepGameLoop::Update(const cppgamelib::DeltaTime* deltaTime)
{
	UpdateFunc(deltaTime);
}

inline void gamelib::FixedStepGameLoop::Draw()
{
	DrawFunc();
}