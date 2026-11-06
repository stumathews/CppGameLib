#pragma once

namespace gamelib {
	class DeltaTime;
}

namespace gamelib
{
	class GameWorldData;

	class IGameLoopStrategy
	{
	public:
		virtual ~IGameLoopStrategy() = default;
		virtual void Update(const gamelib::DeltaTime* deltaTime) = 0;
		virtual void Draw() = 0;
		virtual void Loop(GameWorldData* gameWorldData) = 0;
		
	};
}
