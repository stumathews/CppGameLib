#pragma once
#include "common/Common.h"
#include "events/EventSubscriber.h"
#include "objects/GameWorldData.h"
#include "scene/SceneManager.h"

#include "IGameLoopStrategy.h"

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
		explicit GameStructure(std::function<void(unsigned long deltaMs)> getInputFunction);
		GameStructure(std::shared_ptr<IGameLoopStrategy> gameLoop);
		GameStructure(const GameStructure& other) = delete;
		GameStructure(const GameStructure&& other) = delete;
		GameStructure& operator=(const GameStructure& other) = delete;
		GameStructure& operator=(const GameStructure&& other) = delete;

		// What we do when we unload the Game structure
		~GameStructure() override;

		bool Initialize(int screenWidth, int screenHeight, const std::string& windowTitle, const std::string resourceFilePath, const std::
		                                     string& gameSettingsFilePath);

		/// <summary>
		/// releases resources incl. resource admin and other game subsystems
		/// </summary>
		static bool Unload();

		/// <summary>
		/// Do the Game Loop
		/// </summary>
		/// <returns></returns>
		bool DoGameLoop(const GameWorldData* gameWorldData) const;	
		

	private:

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

		/// <summary>
		/// Initialize SDL
		/// </summary>
		// ReSharper disable once CppInconsistentNaming
		static bool InitializeSdl(int screenWidth, int screenHeight, const std::string& windowTitle);

		/// <summary>
		/// Read keyboard/controller input
		/// </summary>
		void ReadKeyboard(unsigned long deltaMs) const;

		void ReadNetwork() const;

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
		std::function<void(unsigned long deltaMs)> getControllerInputFunction;

		/**
		 * \brief If we should routinely check for keyboard events
		 */
		bool sampleInput{};

		/**
		 * \brief If we should routinely check for network events
		 */
		bool sampleNetwork{};

		std::shared_ptr<IGameLoopStrategy> gameLoop;
	};
}
