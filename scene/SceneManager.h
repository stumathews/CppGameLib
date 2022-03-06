#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <list>
#include "Layer.h"
#include "events/EventSubscriber.h"
#include "resource/ResourceManager.h"
#include "objects/GameWorldData.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>
#include <common/Logger.h>

namespace gamelib
{
	/// <summary>
	/// Represents and manages the current scene
	/// </summary>
	class SceneManager final : public EventSubscriber
	{		
	public:

		/// <summary>
		/// Create a new Scene Manager
		/// </summary>
		SceneManager(EventManager& eventmanager, 
			SettingsManager& settingsManager, 
			ResourceManager& resourceManager, 
			GameWorldData& gameWorld, 
			gamelib::Logger& Logger, 
			std::string scene_folder = "game/");	

		SceneManager(const SceneManager &) = delete;
		SceneManager(SceneManager &&) = delete;
	    SceneManager& operator=(SceneManager const&)  = delete;
		SceneManager& operator=(SceneManager &&) = delete;

		/// <summary>
		/// Initialize the scene manager
		/// </summary>
		/// <returns></returns>
		bool Initialize();	

		/// <summary>
		/// Start the scene
		/// </summary>
		/// <param name="scene_id"></param>
		void StartScene(int scene_id);

		/// <summary>
		/// Get layers
		/// </summary>
		/// <returns></returns>
		std::list<gamelib::Layer> GetLayers() const;

	private:
		/// <summary>
		/// Add game object to scene
		/// </summary>
		/// <param name="game_object"></param>
		void AddGameObjectToScene(const std::shared_ptr<GameObject>& game_object);

		/// <summary>
		/// Load new scene
		/// </summary>
		void LoadNewScene(const std::shared_ptr<Event> &the_event, ResourceManager& resource_admin);

		/// <summary>
		/// Read scene file
		/// </summary>
		/// <param name="filename">filename of the scene file</param>
		bool ReadSceneFile(const std::string &filename, ResourceManager& resource_admin);

		void OnVisibleParse(gamelib::Layer& layer, const std::string& value);
		void OnPosYParse(gamelib::Layer& layer, const std::string& value);
		void OnPosXParse(gamelib::Layer& layer, const std::string& value);
		void OnNameParse(gamelib::Layer& layer, const std::string& value);

		/// <summary>
		/// Remove layer
		/// </summary>
		/// <param name="name"></param>
		void RemoveLayer(const std::string &name);

		/// <summary>
		/// Sort Layer
		/// </summary>
		void SortLayers();
		
		/// <summary>
		/// Add Layer
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		Layer& AddLayer(const std::string &name);
		
		/// <summary>
		/// Find layer
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		Layer& FindLayer(const std::string &name);

		/// <summary>
		/// Update the scene manager on each frame
		/// </summary>
		static void Update();

		/// <summary>
		/// Handle Scene Manager events
		/// </summary>
		/// <param name="the_event"></param>
		/// <returns></returns>
		std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> the_event) override;

		/// <summary>
		/// Remove game object from layer
		/// </summary>
		/// <param name="game_object_id"></param>
		void RemoveGameObjectFromLayers(int game_object_id);

		/// <summary>
		/// Our name for the event system
		/// </summary>
		/// <returns></returns>
		std::string GetSubscriberName() override;
			
		/// <summary>
		/// Layers in this scene
		/// </summary>
		std::list<gamelib::Layer> layers;

		/// <summary>
		/// Current Scene name
		/// </summary>
		std::string currentSceneName = {};

		/// <summary>
		/// True of scene manager is initialized
		/// </summary>
		bool isInitialized = false;

		/// <summary>
		/// Location of the scene file
		/// </summary>
		std::string sceneFolder;

		/// <summary>
		/// Event Manager
		/// </summary>
		EventManager& eventManager;

		/// <summary>
		/// Settings Manager
		/// </summary>
		SettingsManager& settingsManager;

		/// <summary>
		/// Resource manager
		/// </summary>
		ResourceManager& resourceManager;
		
		/// <summary>
		/// Reference to the game world
		/// </summary>
		GameWorldData& gameWorld;

		/// <summary>
		/// Logger
		/// </summary>
		Logger& logger;
	};
}
#endif