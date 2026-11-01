#pragma once

namespace cppgamelib {
	class DeltaTime;
}

namespace gamelib
{
	class GameWorldData;

	class IGameLoopStrategy
	{
	public:
		virtual ~IGameLoopStrategy() = default;
		virtual void Update(const cppgamelib::DeltaTime* deltaTime) = 0;
		virtual void Draw() = 0;
		virtual void Loop(GameWorldData* gameWorldData) = 0;
		
	};
}
