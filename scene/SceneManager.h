#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <list>
#include "Layer.h"
#include "events/EventSubscriber.h"
#include "resource/ResourceManager.h"
#include "objects/GameWorldData.h"
#include <file/Logger.h>

#include "events/EventNumbers.h"

namespace gamelib
{
	class GameWorldData;
	const static EventId DrawCurrentSceneEventId(DrawCurrentScene, "DrawCurrentScene");	
	const static EventId GenerateNewLevelEventId(GenerateNewLevel, "GenerateNewLevel");

	class SceneManager final : public EventSubscriber
	{		
	public:
		static SceneManager* Get();
		~SceneManager() override;
		SceneManager(const SceneManager &) = delete;
		SceneManager(SceneManager &&) = delete;
	    SceneManager& operator=(SceneManager const&)  = delete;
		SceneManager& operator=(SceneManager &&) = delete;

		void SetSceneFolder(const std::string& inSceneFolder);
		bool Initialize(std::string sceneFolder = "game/");
		void StartScene(int sceneId);
		[[nodiscard]] std::list<std::shared_ptr<Layer>> GetLayers() const;

	protected:
		static SceneManager* instance;

	private:
		SceneManager();
		void DrawScene() const;
		void AddGameObjectToScene(const std::shared_ptr<Event>& event) const;
		void LoadNewScene(const std::shared_ptr<Event> &event);
		void OnSceneLoaded(const std::shared_ptr<Event>& event) const;
		static void OnVisibleParse(const std::shared_ptr<Layer>& layer, const std::string& value);
		static void OnPosYParse(const std::shared_ptr<Layer>& layer, const std::string& value);
		static void OnPosXParse(const std::shared_ptr<Layer>&, const std::string& value);
		static void OnNameParse(const std::shared_ptr<Layer>&, const std::string& value);
		void RemoveLayer(const std::string &name);
		void SortLayers();
		static void Update();
		void RemoveGameObjectFromLayers(int gameObjectId);
		std::shared_ptr<Layer> AddLayer(const std::string &name);
		std::shared_ptr<Layer> FindLayer(const std::string &name);
		[[nodiscard]] std::shared_ptr<GameObject> GetGameObjectFrom(const std::shared_ptr<Event>& event) const;
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& event, const unsigned long deltaMs) override;
		[[nodiscard]] std::vector <std::weak_ptr<GameObject>> GetAllObjects() const;
		void UpdateAllObjects(unsigned long deltaMs) const;
		std::string GetSubscriberName() override;
		bool ReadSceneFile(const std::string& filename);
		static bool CompareLayerOrder(const std::shared_ptr<Layer>& rhs, const std::shared_ptr<Layer>& lhs);
		void OnGameObjectEventReceived(const std::shared_ptr<Event>& event);

		std::list<std::shared_ptr<Layer>> layers;
		std::string currentSceneName = {};
		bool isInitialized = false;
		std::string sceneFolder;
		GameWorldData gameWorld;
	};
}
#endif