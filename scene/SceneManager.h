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
	class GameWorldData;
	/// <summary>
	/// Represents and manages the current scene
	/// </summary>
	class SceneManager final : public EventSubscriber
	{		
	public:

		/// <summary>
		/// Get Scene Manager
		/// </summary>
		static SceneManager* Get();
		~SceneManager();
		SceneManager(const SceneManager &) = delete;
		SceneManager(SceneManager &&) = delete;
	    SceneManager& operator=(SceneManager const&)  = delete;
		SceneManager& operator=(SceneManager &&) = delete;

		GameWorldData& GetGameWorld();

		void SetSceneFolder(std::string sceneFolder);

		/// <summary>
		/// Initialize the scene manager
		/// </summary>
		/// <returns></returns>
		bool Initialize(std::string sceneFolder = "game/");

		/// <summary>
		/// Start the scene
		/// </summary>
		/// <param name="scene_id"></param>
		void StartScene(int scene_id);

		/// <summary>
		/// Get layers
		/// </summary>
		/// <returns></returns>
		std::list<std::shared_ptr<gamelib::Layer>> GetLayers() const;


	protected:
		static SceneManager* Instance;

	private:
		SceneManager();
		
		/// <summary>
		/// Draws the current scene
		/// </summary>
		void DrawScene();
		
		/// <summary>
		/// Add game object to scene
		/// </summary>
		/// <param name="game_object"></param>
		void AddGameObjectToScene(const std::shared_ptr<GameObject>& game_object);

		/// <summary>
		/// Load new scene
		/// </summary>
		void LoadNewScene(const std::shared_ptr<Event> &the_event);

		/// <summary>
		/// Read scene file
		/// </summary>
		/// <param name="filename">filename of the scene file</param>
		bool ReadSceneFile(const std::string &filename);

		void OnVisibleParse(std::shared_ptr<gamelib::Layer> layer, const std::string& value);
		void OnPosYParse(std::shared_ptr<gamelib::Layer> layer, const std::string& value);
		void OnPosXParse(std::shared_ptr<gamelib::Layer>, const std::string& value);
		void OnNameParse(std::shared_ptr<gamelib::Layer>, const std::string& value);

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
		std::shared_ptr<Layer> AddLayer(const std::string &name);
		
		/// <summary>
		/// Find layer
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		std::shared_ptr<Layer> FindLayer(const std::string &name);

		/// <summary>
		/// Update the scene manager on each frame
		/// </summary>
		void Update();

		

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
		std::list<std::shared_ptr<gamelib::Layer>> layers;

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
		/// Reference to the game world
		/// </summary>
		GameWorldData gameWorld;
	};
}
#endif