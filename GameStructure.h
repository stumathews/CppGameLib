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
		explicit GameStructure(std::function<void()> getControllerInputFunction);

		// What we do when we unload the Game structure
		~GameStructure() override;

		static bool InitializeGameSubSystems(int screenWidth, int screenHeight, const std::string& windowTitle, std::string resourceFilePath);

		/// <summary>
		/// releases resources incl. resource admin and other game subsystems
		/// </summary>
		static bool UnloadGameSubsystems();

		/// <summary>
		/// Do the Game Loop
		/// </summary>
		/// <returns></returns>
		bool DoGameLoop(const GameWorldData* gameWorldData) const;
		
		/// <summary>
		/// Draw the game 
		/// </summary>
		void Draw(unsigned long) const;

		/// <summary>
		/// Update the game
		/// </summary>
		void Update(unsigned long deltaMs) const;
		
		/// <summary>
		/// Handle any game Structure events
		/// </summary>
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs) override;

		/// <summary>
		/// Name we register with the event system
		/// </summary>
		/// <returns></returns>
		std::string GetSubscriberName() override;

	private:		

		/// <summary>
		/// Initialize SDL
		/// </summary>
		// ReSharper disable once CppInconsistentNaming
		static bool InitializeSDL(int screenWidth, int screenHeight, const std::string& windowTitle);

		/// <summary>
		/// Read keyboard/controller input
		/// </summary>
		void ReadKeyboard() const;

		static void ReadNetwork();

		/// <summary>
		/// Gte the Time now in MS
		/// </summary>
		static long GetTimeNowMs();

		/// <summary>
		/// Spare time (Not drawing and not updating)
		/// </summary>
		static void HandleSpareTime(long);

		/// <summary>
		/// Input function (get physical player/controller input)
		/// </summary>
		std::function<void()> _getControllerInputFunction;
	};
}
