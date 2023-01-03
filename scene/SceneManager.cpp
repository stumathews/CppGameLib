#include "SceneManager.h"
#include <list>
#include "tinyxml2.h"
#include <memory>
#include <list>
#include "events/GameObjectEvent.h"
#include "common/Common.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/EventManager.h"
#include "events/SceneChangedEvent.h"
#include "events/SceneLoadedEvent.h"
#include "objects/GameObjectFactory.h"
#include "util/SettingsManager.h"

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Create a Scene Manager.
	/// <remarks>A scene is a collection of objects. A scene is loaded from a file</remarks>
	/// </summary>
	SceneManager::SceneManager() { }
	SceneManager* SceneManager::Get() { if (Instance == nullptr) { Instance = new SceneManager(); } return Instance; }
	SceneManager::~SceneManager() { Instance = nullptr; }
		
	void SceneManager::AddGameObjectToScene(const shared_ptr<Event>& event) const { layers.back()->Objects.push_back(GetGameObjectFrom(event)); }
	void SceneManager::SortLayers() 
	{ 
		layers.sort([=](const shared_ptr<Layer>& rhs, const shared_ptr<Layer>& lhs) { return lhs->Zorder < rhs->Zorder; });
	}
	void SceneManager::Update() { }
	void SceneManager::SetSceneFolder(const string& inSceneFolder) { this->sceneFolder = inSceneFolder; }
	void SceneManager::OnVisibleParse(const shared_ptr<Layer>& layer, const string& value) { layer->Visible = (value == "true") ? true : false; }
	void SceneManager::OnPosYParse(const shared_ptr<Layer>& layer, const string& value) { layer->Position.SetY(static_cast<int>(atoi(value.c_str()))); }
	void SceneManager::OnPosXParse(const shared_ptr<Layer>& layer, const string& value) { layer->Position.SetX(static_cast<int>(atoi(value.c_str()))); }
	void SceneManager::OnNameParse(const shared_ptr<Layer>& layer, const string& value) { layer->SetName(value); }
	void SceneManager::OnSceneLoaded(const std::shared_ptr<Event>& event) const { LogMessage("Scene " + to_string(dynamic_pointer_cast<SceneChangedEvent>(event)->SceneId) + " loaded."); }
	bool SceneManager::CompareLayerOrder(const shared_ptr<Layer>& rhs, const shared_ptr<Layer>& lhs) { return lhs->Zorder < rhs->Zorder; }
	list<shared_ptr<Layer>> SceneManager::GetLayers() const { return layers; }
	string SceneManager::GetSubscriberName() { return "SceneManager"; }

	bool SceneManager::Initialize(const string inSceneFolder)
	{
		isInitialized = LogThis("SceneManager::initialize()", SettingsManager::Get()->GetBool("global", "verbose"), [&]()
		{
			SetSceneFolder(inSceneFolder);

			EventManager::Get()->SubscribeToEvent(EventType::LevelChangedEventType, this);
			EventManager::Get()->SubscribeToEvent(EventType::AddGameObjectToCurrentScene, this);
			EventManager::Get()->SubscribeToEvent(EventType::GenerateNewLevel, this);
			EventManager::Get()->SubscribeToEvent(EventType::GameObject, this);
			EventManager::Get()->SubscribeToEvent(EventType::DrawCurrentScene, this);
			EventManager::Get()->SubscribeToEvent(EventType::UpdateAllGameObjectsEventType, this);

			return true;
		}, true, true);
		return isInitialized;
	}

	vector<shared_ptr<Event>> SceneManager::HandleEvent(const shared_ptr<Event> event, const unsigned long deltaMs)
	{
		vector<shared_ptr<Event>> secondaryEvents;

		// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
		// ReSharper disable once CppIncompleteSwitchStatement
		switch(event->Type)  // NOLINT(clang-diagnostic-switch)
		{
			case EventType::DrawCurrentScene: DrawScene(); break;
			case EventType::LevelChangedEventType: LoadNewScene(event); break;
			case EventType::UpdateAllGameObjectsEventType: 	UpdateAllObjects(deltaMs); break;
			case EventType::AddGameObjectToCurrentScene: AddGameObjectToScene(event); break;
			case EventType::GameObject: OnGameObjectEventReceived(event); break;
			case EventType::SceneLoaded: OnSceneLoaded(event); break;
		}
		
		// We dont generate any events yet;
		return secondaryEvents;
	}

	void SceneManager::UpdateAllObjects(const unsigned long deltaMs) const
	{
		for (auto& object : GetAllObjects()) 
		{
			if (const auto ptr = object.lock()) { ptr->Update(static_cast<float>(deltaMs)); }
		}
	}

	void SceneManager::OnGameObjectEventReceived(const shared_ptr<Event>& event)
	{
		const auto gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(event);

		if (gameObjectEvent->Context == GameObjectEventContext::Remove)
		{
			if (gameObjectEvent->GameObject) { RemoveGameObjectFromLayers(gameObjectEvent->GameObject->Id); }
		}
	}

	void SceneManager::RemoveGameObjectFromLayers(int GameObjectId)
	{
		// Remove from each layer, the occurnace of the object denoted by gameObjectId
		for_each(begin(layers), end(layers), [&GameObjectId](const shared_ptr<Layer>& layer)
		{
			// Look for game object that match incomming id
			auto result = remove_if(begin(layer->Objects), end(layer->Objects), [=](const weak_ptr<GameObject>&
			                        gameObject)
			{
				if (const auto ptr = gameObject.lock()) { return ptr->Id == GameObjectId; }
				return false;
			});			
		});
	}

	void SceneManager::LoadNewScene(const shared_ptr<Event>& event)
	{
		string sceneFileName;		
		const auto scene = dynamic_pointer_cast<SceneChangedEvent>(event)->SceneId;

		switch(scene)
		{
			case 1: sceneFileName = sceneFolder + "scene1.xml"; break;
			case 2: sceneFileName = sceneFolder + "scene2.xml"; break;
			case 3: sceneFileName = sceneFolder + "scene3.xml"; break;
			case 4: sceneFileName = sceneFolder + "scene4.xml"; break;
			default: sceneFileName = sceneFolder + "scene1.xml"; break;
		}

		if (sceneFileName.empty()) { return; }

		try 
		{
			if (ReadSceneFile(sceneFileName)) { EventManager::Get()->RaiseEvent(make_unique<SceneLoadedEvent>(scene), this); }
		} 
		catch(exception &e)
		{
			THROW(13, string("Cloud not load scene file: ") + string(e.what()), GetSubscriberName())
		}		
	}



	shared_ptr<Layer> SceneManager::AddLayer(const string& name)
	{
		shared_ptr<Layer> layer = std::make_shared<Layer>();
		layer->SetName(name);
		layers.push_back(layer);

		return layer;
	}

	shared_ptr<Layer> SceneManager::FindLayer(const string& name)
	{
		for (auto& layer : layers)
		{
			if (layer->Name() == name) { return layer; }
		}
		
		THROW(12, string("layer not found:" + name), GetSubscriberName())
	}

	void SceneManager::RemoveLayer(const string& name) 
	{
		remove_if(begin(layers), end(layers), [&name](const shared_ptr<Layer>& layer)  // NOLINT(bugprone-unused-return-value, clang-diagnostic-unused-result)
		{
			return layer->Name()._Equal(name);
		});
	}

	void SceneManager::DrawScene() const
	{
		const auto renderAllObjectsFn = static_cast<RenderFunc>([&](SDL_Renderer* windowRenderer)
		{
			// Draw all objects in the layer
			for (const auto& layer : GetLayers())
			{
				// Only draw visible layers
				if (layer->Visible)
				{
					// Draw objects within the layer
					for (const auto& gameObject : layer->Objects)
					{
						// If it is active
						if (const auto ptr = gameObject.lock())
						{
							// draw yourself!
							ptr->Draw(windowRenderer);
						}						
					}
				}
			}
		});

		SDLGraphicsManager::Get()->ClearAndDraw(renderAllObjectsFn);
	}

	std::vector <std::weak_ptr<GameObject>> SceneManager::GetAllObjects() const
	{
		std::vector<std::weak_ptr<GameObject>> gameObjects;
		for (const auto& layer : GetLayers())
		{
			for (const auto& gameObject : layer->Objects)
			{
				gameObjects.push_back(gameObject);				
			}			
		}
		return gameObjects;
	}

	bool SceneManager::ReadSceneFile(const string& filename)
	{
		if(currentSceneName == filename) { LogMessage(string("Scene already loaded. Skipping.")); return true; }

		LogMessage("Loading scene: " + string(filename));
		
		/* Eg. 

			<scene id="2">
			  <layer name="layer0" posx="0" posy="0" visible="true">
				<objects>
				  <object posx="100" posy="40" resourceId="7" visible="true" colourKey="true" r="0" g="0" b="0"></object>
				  <object posx="500" posy="40" resourceId="8" visible="true" colourKey="true" r="0" g="0" b="0"></object>
				</objects>
			  </layer>
			</scene>

		*/

		tinyxml2::XMLDocument doc;		

		doc.LoadFile(filename.c_str());

		if(doc.ErrorID() == 0) 	
		{
			auto* scene = doc.FirstChildElement("scene");
			

			if(scene) // <scene id="2">
			{
				for(auto layerNode = scene->FirstChild(); layerNode; layerNode = layerNode->NextSibling()) //  <layer ...>
				{				
					auto layerElement = layerNode->ToElement();
					if(layerElement) 
					{
						// build up a layer object from scene file
						shared_ptr<Layer> currentLayer = std::make_shared<Layer>();
						currentLayer->Zorder = layers.size();					

						// Loop through layer attributes
						for(auto layerAttributes = layerElement->FirstAttribute(); layerAttributes; layerAttributes = layerAttributes->Next()) // // <layer name="layer0" posx="0" posy="0" visible="true"
						{						
							// populate the new layer object:
							
							const string name(layerAttributes->Name());
							const string value(layerAttributes->Value());						

							if (name == "name") { OnNameParse(currentLayer, value); continue; }
							if (name == "posx") { OnPosXParse(currentLayer, value); continue; }
							if (name == "posy") { OnPosYParse(currentLayer, value); continue; }
							if (name == "visible") { OnVisibleParse(currentLayer, value); continue; }
						}

						// Process inner contents of the layer 
						for(auto layerContent = layerNode->FirstChild(); layerContent; layerContent = layerContent->NextSibling()) 
						{							
							if(string(layerContent->Value()) == "objects") 
							{
								// <object ...
								for(auto objectNode = layerContent->FirstChild(); objectNode; objectNode = objectNode->NextSibling())
								{	
									auto objectElement = objectNode->ToElement();

									if(objectElement == nullptr) { Logger::Get()->LogThis(string("Invalid or null object found in scene file")); continue; }
																		
									// Build GameObject from <object>
									auto gameObject = GameObjectFactory::Get().BuildGameObject(objectElement);

									// Add game object to this layer
									currentLayer->Objects.push_back(gameObject);
								}
							}
						}

						// Add this layer to scene
						layers.push_back(currentLayer);
					}
				}

				// We want to draw from zOrder 0 -> onwards (in order)
				SortLayers(); 

				// Remember what scene we are currently in
				currentSceneName = filename;
				return true;
			} // finished processing scene, layers populated
		}
		return false;
	}

	std::shared_ptr<GameObject> SceneManager::GetGameObjectFrom(const std::shared_ptr<Event>& event) const
	{
		if (event->Type != EventType::AddGameObjectToCurrentScene) { 	THROW(1, "Cannot extract game object from event", "SceneManager")
		}

		return dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(event)->GetGameObject();
	}

	void SceneManager::StartScene(int scene_id)
	{
		// This triggers usually the ResourceManager that loads the resources in for the scene (see ResourceManager::process_events etc..)
		EventManager::Get()->RaiseEvent(make_shared<SceneChangedEvent>(scene_id), this);
	}

	SceneManager* SceneManager::Instance = nullptr;
}
