#pragma once
#include <functional>

#include "structure/IGameLoopStrategy.h"

namespace gamelib
{
	class FixedStepGameLoop : public IGameLoopStrategy
	{
	public:
		FixedStepGameLoop(unsigned int timeStepMs,
			std::function<void(const cppgamelib::DeltaTime* deltaTime)> updateFunc,
			std::function<void()> drawFunc,
			std::function<void(const cppgamelib::DeltaTime* deltaTime)> inputFunc);
		void Loop(GameWorldData* gameWorldData) override;
	private:
		static long GetTimeNowMs();
		void Update(const cppgamelib::DeltaTime* deltaTime) override;
		void Draw() override;
		std::function<void(const cppgamelib::DeltaTime* deltaTime)> UpdateFunc;
		std::function<void()> DrawFunc;
		std::function<void(const cppgamelib::DeltaTime* deltaTime)> InputFunc;
		const unsigned TimeStepMs;
	};
}

