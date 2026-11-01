#pragma once

#include <functional>

#include "events/EventSubscriber.h"
#include "time/DeltaTime.h"

namespace gamelib
{
	class IGameLoopStrategy;
	class VariableGameLoop;
	class GameWorldData;

	/// <summary>
	/// logical structure of the game such as the game initialization, game loop etc.
	/// </summary>
	class GameStructure final : public EventSubscriber
	{
	public:

		GameStructure(const GameStructure& other) = delete;
		GameStructure(const GameStructure&& other) = delete;
		GameStructure& operator=(const GameStructure& other) = delete;
		GameStructure& operator=(const GameStructure&& other) = delete;

		// Use default game loop
		explicit GameStructure(std::function<void(const cppgamelib::DeltaTime* deltaTime)> getInputFunction);

		// Use the specified game loop
		explicit GameStructure(std::shared_ptr<IGameLoopStrategy> gameLoop);

		// Initialize all dependencies and requirements
		bool Initialize(int screenWidth, int screenHeight, const std::string &windowTitle,
		                const std::string &resourceFilePath, const std::string &sceneFolderPath,
		                bool hideWindow );

		// Finish up and unload the Game structure
		~GameStructure() override;

		// Releases resources incl. resource admin and other game subsystems
		static bool Unload();

		// Do the Game Loop
		bool DoGameLoop(GameWorldData* gameWorldData) const;	
		

	private:

		[[nodiscard]] std::shared_ptr<VariableGameLoop> MakeVariableGameLoop() const;

		// Draw the game
		static void Draw(unsigned long);

		// Update the game
		void Update(const cppgamelib::DeltaTime* deltaTime) const;
		
		// Handle any game Structure events
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& event, const cppgamelib::DeltaTime* deltaTime) override;

		// Name we register with the event system
		std::string GetSubscriberName() override;

		// Initialize SDL
		static bool InitializeSdl(int screenWidth, int screenHeight, const std::string& windowTitle, const bool hideWindow);

		// Read keyboard/controller input
		void ReadKeyboard(const cppgamelib::DeltaTime* deltaTime) const;

		// Read from the network
		void ReadNetwork(const cppgamelib::DeltaTime* deltaTime) const;

		// Get the Time now in MS
		static long GetTimeNowMs();

		// Spare time (Not drawing and not updating)
		static void HandleSpareTime(long);

		// Store the input function (get physical player/controller input)
		std::function<void(const cppgamelib::DeltaTime* deltaTime)> getControllerInputFunction;

		// If we should routinely check for keyboard events
		bool sampleInput{};

		// If we should routinely check for network events
		bool sampleNetwork{};

		// Reference to the provided game loop
		std::shared_ptr<IGameLoopStrategy> gameLoop;
	};
}
