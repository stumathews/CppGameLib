#pragma once

#include <functional>

#include "events/EventSubscriber.h"

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
		explicit GameStructure(std::function<void(unsigned long deltaMs)> getInputFunction);

		// Use the specified game loop
		explicit GameStructure(std::shared_ptr<IGameLoopStrategy> gameLoop);

		// Initialize all dependnecies and requirements
		bool Initialize(int screenWidth, int screenHeight, const std::string &windowTitle,
		                const std::string &resourceFilePath, const std::string &sceneFolderPath);

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
		void Update(unsigned long deltaMs) const;
		
		// Handle any game Structure events
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& event, unsigned long deltaMs) override;

		// Name we register with the event system
		std::string GetSubscriberName() override;

		// Initialize SDL
		static bool InitializeSdl(int screenWidth, int screenHeight, const std::string& windowTitle);

		// Read keyboard/controller input
		void ReadKeyboard(unsigned long deltaMs) const;

		// Read from the network
		void ReadNetwork(unsigned long deltaMs) const;

		// Get the Time now in MS
		static long GetTimeNowMs();

		// Spare time (Not drawing and not updating)
		static void HandleSpareTime(long);

		// Store the input function (get physical player/controller input)
		std::function<void(unsigned long deltaMs)> getControllerInputFunction;

		// If we should routinely check for keyboard events
		bool sampleInput{};

		// If we should routinely check for network events
		bool sampleNetwork{};

		// Reference to the provided game loop
		std::shared_ptr<IGameLoopStrategy> gameLoop;
	};
}
