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

	class SceneManager final : public EventSubscriber
	{		
	public:
		static SceneManager* Get();
		~SceneManager();
		SceneManager(const SceneManager &) = delete;
		SceneManager(SceneManager &&) = delete;
	    SceneManager& operator=(SceneManager const&)  = delete;
		SceneManager& operator=(SceneManager &&) = delete;

		GameWorldData& GetGameWorld();
		void SetSceneFolder(std::string sceneFolder);
		bool Initialize(std::string sceneFolder = "game/");
		void StartScene(int scene_id);
		std::list<std::shared_ptr<gamelib::Layer>> GetLayers() const;

	protected:
		static SceneManager* Instance;

	private:
		SceneManager();
		void DrawScene();
		void AddGameObjectToScene(std::shared_ptr<Event> event);
		void LoadNewScene(const std::shared_ptr<Event> &the_event);
		void OnVisibleParse(std::shared_ptr<gamelib::Layer> layer, const std::string& value);
		void OnPosYParse(std::shared_ptr<gamelib::Layer> layer, const std::string& value);
		void OnPosXParse(std::shared_ptr<gamelib::Layer>, const std::string& value);
		void OnNameParse(std::shared_ptr<gamelib::Layer>, const std::string& value);
		void RemoveLayer(const std::string &name);
		void SortLayers();	
		void Update();
		void RemoveGameObjectFromLayers(int game_object_id);
		std::shared_ptr<Layer> AddLayer(const std::string &name);
		std::shared_ptr<Layer> FindLayer(const std::string &name);		
		std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> the_event, unsigned long deltaMs) override;
		void UpdateAllObjects(unsigned long deltaMs);
		std::string GetSubscriberName() override;
		bool ReadSceneFile(const std::string& filename);
		void OnGameObjectEventReceived(std::shared_ptr<gamelib::Event> event);

		std::list<std::shared_ptr<gamelib::Layer>> layers;
		std::string currentSceneName = {};
		bool isInitialized = false;
		std::string sceneFolder;
		GameWorldData gameWorld;
	};
}
#endif