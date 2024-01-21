#pragma once

namespace gamelib
{
	class GameWorldData;

	class IGameLoopStrategy
	{
	public:
		virtual ~IGameLoopStrategy() = default;
		virtual void Update(unsigned long deltaMs) = 0;
		virtual void Draw() = 0;
		virtual void Loop(GameWorldData* gameWorldData) = 0;
		
	};
}