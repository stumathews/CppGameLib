#pragma once
#include <functional>

#include "structure/IGameLoopStrategy.h"

namespace gamelib
{
	class FixedStepGameLoop : public IGameLoopStrategy
	{
	public:
		FixedStepGameLoop(unsigned int timeStepMs,
			std::function<void(unsigned long deltaMs)> updateFunc, 
			std::function<void()> drawFunc,
			std::function<void(unsigned long deltaMs)> inputFunc);

		static long GetTimeNowMs();
		void Update(unsigned long deltaMs) override;
		void Draw() override;
		void Loop(const GameWorldData* gameWorldData) override;
		std::function<void(unsigned long deltaMs)> UpdateFunc;
		std::function<void()> DrawFunc;
		std::function<void(unsigned long deltaMs)> InputFunc;
		const unsigned TimeStepMs;
	};
}

