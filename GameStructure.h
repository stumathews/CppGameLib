#pragma once
#include "common/Common.h"
#include "events/EventSubscriber.h"
#include "objects/GameWorldData.h"
#include "objects/Player.h"
#include "scene/SceneManager.h"
#include <common/Logger.h>

namespace gamelib
{
	/***
	 * logical structure of the game such as the game initialization, game loop etc.
	 */
	class GameStructure final : public EventSubscriber
	{
	public:
		GameStructure(EventManager& eventManager, ResourceManager& resource_admin, 
		              SettingsManager& config, GameWorldData& gameWorld,
		              SceneManager& scene_admin, SDLGraphicsManager& graphics_admin,
		              AudioManager& audio_admin, std::function<void()> get_input_func,
					  Logger& gameLogger);
		~GameStructure();

		bool InitializeSDL(int screen_width, int screen_height);	
		bool InitializeGameSubSystems();

		bool DoGameLoop();
		
		void Draw(float);
		void Update();
		void UpdateWorld() const;

		void ReadKeyboard();		

		static long GetTimeNowMs();
		void HandleSpareTime(long) const;

		// releases resources incl. resource admin and other game subsystems
		bool UnloadGameSubsystems();	 
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> the_event) override;
		std::string GetSubscriberName() override;

	private:		
		EventManager& _eventManager;
		ResourceManager& _resourceManager;
		SettingsManager& _settingsManager;		
		SceneManager& _sceneManager;
		SDLGraphicsManager& _graphicsManager;
		GameWorldData& _gameWorld;
		AudioManager& _audioManager;
		std::function<void()> _getControllerInputFunction;
		Logger& _gameLogger;
	};
}
