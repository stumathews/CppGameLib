#pragma once
#include <functional>

#include "IGameLoopStrategy.h"
#include "objects/GameWorldData.h"

namespace gamelib
{
	class VariableGameLoop final : public IGameLoopStrategy
	{
		
	public:
		
		VariableGameLoop(std::function<void(const cppgamelib::DeltaTime* deltaTime)> updateFunc, std::function<void()> drawFunc);
		void Update(const cppgamelib::DeltaTime* deltaTime) override;
		void Draw() override;
		void Loop(GameWorldData* gameWorldData) override;
	private:
		std::function<void(const cppgamelib::DeltaTime* deltaTime)> updateFunction;
		std::function<void()> drawFunction;

		std::function<void(const cppgamelib::DeltaTime* deltaTime)> UpdateFunc;
		std::function<void()> drawFunc;
		GameWorldData gameWorldData;
	};
}

