#pragma once
#include "common/Common.h"
#include "events/EventSubscriber.h"
#include "objects/GameWorldData.h"
#include "objects/Player.h"
#include "scene/SceneManager.h"
#include <common/Logger.h>

namespace gamelib
{
	/// <summary>
	/// logical structure of the game such as the game initialization, game loop etc.
	/// </summary>
	class GameStructure final : public EventSubscriber
	{
	public:

		/// <summary>
		/// Create a game structure
		/// </summary>
		/// <param name="gameWorld">gameWorld data</param>
		/// <param name="get_input_func">Controller/Player Input function</param>
		GameStructure(GameWorldData& world, SceneManager& sceneManager, std::function<void()> getControllerInputFunction);

		// What we do when we unload the Gamestructure
		~GameStructure();
	
		/// <summary>
		/// Initialize Game Subsystems
		/// </summary>
		/// <returns></returns>
		bool InitializeGameSubSystems();

		/// <summary>
		/// releases resources incl. resource admin and other game subsystems
		/// </summary>
		bool UnloadGameSubsystems();

		/// <summary>
		/// Do the Game Loop
		/// </summary>
		/// <returns></returns>
		bool DoGameLoop();
		
		/// <summary>
		/// Draw the game 
		/// </summary>
		/// <param name=""></param>
		void Draw(float);

		/// <summary>
		/// Update the game
		/// </summary>
		void Update();
		
		/// <summary>
		/// Handle any game Structure events
		/// </summary>
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event) override;

		/// <summary>
		/// Name we register with the event system
		/// </summary>
		/// <returns></returns>
		std::string GetSubscriberName() override;

	private:		

		/// <summary>
		/// Initialize SDL
		/// </summary>
		bool InitializeSDL(int screen_width, int screen_height);

		/// <summary>
		/// Update World/level etc
		/// </summary>
		void UpdateWorld() const;

		/// <summary>
		/// Read keyboard/controller input
		/// </summary>
		void ReadKeyboard() const;

		/// <summary>
		/// Gte the Time now in MS
		/// </summary>
		static long GetTimeNowMs();

		/// <summary>
		/// Spare time (Not drawing and not updating)
		/// </summary>
		void HandleSpareTime(long) const;
				
				/// <summary>
		/// Scene Manager
		/// </summary>
		SceneManager& _sceneManager;

		/// <summary>
		/// GameWorld
		/// </summary>
		GameWorldData& _gameWorld;

		/// <summary>
		/// Input function (get physical player/controller input)
		/// </summary>
		std::function<void()> _getControllerInputFunction;
	};
}
