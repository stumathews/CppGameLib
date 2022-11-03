#pragma once
#include "common/Common.h"
#include "events/EventSubscriber.h"
#include "objects/GameWorldData.h"
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
		GameStructure(std::function<void()> getControllerInputFunction);

		// What we do when we unload the Gamestructure
		~GameStructure();
		
		bool InitializeGameSubSystems(int screenWidth, int screenHeight, std::string windowTitle, std::string resourceFilePath);

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
		void Update(float deltaMs);
		
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
		bool InitializeSDL(int screen_width, int screen_height, std::string windowTitle);

		/// <summary>
		/// Read keyboard/controller input
		/// </summary>
		void ReadKeyboard() const;

		void ReadNetwork() const;

		/// <summary>
		/// Gte the Time now in MS
		/// </summary>
		static long GetTimeNowMs();

		/// <summary>
		/// Spare time (Not drawing and not updating)
		/// </summary>
		void HandleSpareTime(long) const;

		/// <summary>
		/// Input function (get physical player/controller input)
		/// </summary>
		std::function<void()> _getControllerInputFunction;
	};
}
