#pragma once
#include <functional>

#include "IGameLoopStrategy.h"
#include "objects/GameWorldData.h"

namespace gamelib
{
	class VariableGameLoop final : public IGameLoopStrategy
	{
		
	public:
		
		VariableGameLoop(std::function<void(unsigned long deltaMs)> updateFunc, std::function<void()> drawFunc);
		void Update(unsigned long deltaMs) override;
		void Draw() override;
		void Loop(const GameWorldData* gameWorldData) override;
	private:
		std::function<void(unsigned long deltaMs)> updateFunction;
		std::function<void()> drawFunction;

		std::function<void(unsigned long deltaMs)> updateFunc;
		std::function<void()> drawFunc;
		GameWorldData gameWorldData;
	};
}

