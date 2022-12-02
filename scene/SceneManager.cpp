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
#include <events/UpdateAllGameObjectsEvent.h>

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
	
	void SceneManager::SetSceneFolder(string sceneFolder) 	{ this->sceneFolder = sceneFolder; }
	void SceneManager::OnVisibleParse(shared_ptr<Layer> layer, const string& value) { layer->visible = (value == "true") ? true : false; }
	void SceneManager::OnPosYParse(shared_ptr<Layer> layer, const string& value) { layer->Position.SetY(static_cast<int>(atoi(value.c_str()))); }
	void SceneManager::OnPosXParse(shared_ptr<Layer> layer, const string& value) { layer->Position.SetX(static_cast<int>(atoi(value.c_str()))); }
	void SceneManager::OnNameParse(shared_ptr<Layer> layer, const string& value) { layer->SetName(value); }
	void SceneManager::AddGameObjectToScene(const shared_ptr<Event> event) 
	{ 
		
		layers.back()->Objects.push_back(dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(event)->GetGameObject());
	}
	bool CompareLayerOrder(const shared_ptr<Layer> rhs, const shared_ptr<Layer> lhs) { return lhs->zorder < rhs->zorder; }
	void SceneManager::SortLayers() { layers.sort(CompareLayerOrder); }
	void SceneManager::Update() { }
	
	GameWorldData& SceneManager::GetGameWorld() { return gameWorld; }
	list<shared_ptr<Layer>> SceneManager::GetLayers() const { return layers; }
	string SceneManager::GetSubscriberName() { return "SceneManager"; }

	bool SceneManager::Initialize(string sceneFolder)
	{
		isInitialized = LogThis("SceneManager::initialize()", SettingsManager::Get()->GetBool("global", "verbose"), [&]()
		{
			SetSceneFolder(sceneFolder);

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


	vector<shared_ptr<Event>> SceneManager::HandleEvent(const shared_ptr<Event> event, unsigned long deltaMs)
	{
		vector<shared_ptr<Event>> secondaryEvents;

		switch(event->type)
		{
			case EventType::ControllerMoveEvent: break;			
			case  EventType::DrawCurrentScene: DrawScene(); break;
			case EventType::LevelChangedEventType: LoadNewScene(event); break;
			case EventType::UpdateAllGameObjectsEventType: 	UpdateAllObjects(deltaMs); break;
			case EventType::AddGameObjectToCurrentScene: AddGameObjectToScene(event); break;
			case EventType::PlayerMovedEventType: break;
			case EventType::SceneLoaded: break;
			case EventType::GameObject: OnGameObjectEventReceived(event); break;
		}
		
		// We dont generate any events yet;
		return secondaryEvents;
	}

	void SceneManager::UpdateAllObjects(unsigned long deltaMs)
	{
		for (auto& object : GetGameWorld().GetGameObjects()) { object->Update(deltaMs); }
	}

	void SceneManager::OnGameObjectEventReceived(shared_ptr<gamelib::Event> event)
	{
		const auto gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(event);

		if (gameObjectEvent->context == GameObjectEventContext::Remove)
		{
			if (gameObjectEvent->gameObject) { RemoveGameObjectFromLayers(gameObjectEvent->gameObject->Id); }
		}
	}

	void SceneManager::RemoveGameObjectFromLayers(int GameObjectId)
	{
		// Remove from each layer, the occurnace of the object denoted by gameObjectId
		for_each(begin(layers), end(layers), [&GameObjectId](shared_ptr<Layer> layer)
		{
			// Look for game object that match incomming id
			auto result = remove_if(begin(layer->Objects), end(layer->Objects), [=](weak_ptr<GameObject> gameObject)
			{
				if (auto ptr = gameObject.lock()) { return ptr->Id == GameObjectId; }
				return false;
			});			
		});
	}

	void SceneManager::LoadNewScene(const shared_ptr<Event>& event)
	{
		// This will raise the event 
		auto raiseSceneLoadedEventFunction = [this](int scene_id, const string& scene_name)
		{
			LogMessage("Scene "+ to_string(scene_id) +" : "+ scene_name +" loaded.");
			EventManager::Get()->RaiseEvent(make_unique<SceneLoadedEvent>(scene_id), this);
		};

		string scene_name;
		
		const auto scene = dynamic_pointer_cast<SceneChangedEvent>(event)->scene_id;

		switch(scene)
		{
			case 1: scene_name = sceneFolder + "scene1.xml"; break;
			case 2: scene_name = sceneFolder + "scene2.xml"; break;
			case 3: scene_name = sceneFolder +"scene3.xml"; break;
			case 4: scene_name = sceneFolder + "scene4.xml"; break;
			default: scene_name = sceneFolder + "scene1.xml"; break;
		}

		if(!scene_name.empty())
		{
			try 
			{
				if (ReadSceneFile(scene_name)) { raiseSceneLoadedEventFunction(scene, scene_name); }
			} 
			catch(exception &e)
			{
				THROW(13, string("Cloud not load scene file: ") + string(e.what()), GetSubscriberName());
			}
		}
	}

	shared_ptr<Layer> SceneManager::AddLayer(const string& name)
	{
		shared_ptr<Layer> layer = shared_ptr<Layer>(new Layer());
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
		
		THROW(12, string("layer not found:" + name), GetSubscriberName());
	}

	void SceneManager::RemoveLayer(const string& name) 
	{
		remove_if(begin(layers), end(layers), [&name](shared_ptr<Layer> layer) { return layer->Name()._Equal(name);  });
	}

	void SceneManager::DrawScene()
	{
		auto renderAllObjectsFn = static_cast<render_func>([&](SDL_Renderer* windowRenderer)
		{
			// Draw all objects in the layer
			for (const auto& Layer : GetLayers())
			{
				// Only draw visible layers
				if (Layer->visible)
				{
					// Draw objects within the layer
					for (const auto& gameObjectCandidate : Layer->Objects)
					{
						if (auto gameObject = gameObjectCandidate.lock())
						{
							// If it is active
							if (gameObject && gameObject->IsActive)
							{
								// draw yourself!
								gameObject->Draw(windowRenderer);
							}
						}
					}
				}
			}
		});

		SDLGraphicsManager::Get()->ClearAndDraw(renderAllObjectsFn);
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
			auto scene_id = scene->ToElement()->Attribute("id");
			
			if(scene) // <scene id="2">
			{
				for(auto layerNode = scene->FirstChild(); layerNode; layerNode = layerNode->NextSibling()) //  <layer ...>
				{				
					auto layerElement = layerNode->ToElement();
					if(layerElement) 
					{
						// build up a layer object from scene file
						shared_ptr<Layer> currentLayer = shared_ptr<Layer>(new Layer());
						currentLayer->zorder = layers.size();					

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
									gameWorld.GetGameObjects().push_back(gameObject);

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

	void SceneManager::StartScene(int scene_id)
	{
		// This triggers usually the ResourceManager that loads the resources in for the scene (see ResourceManager::process_events etc..)
		EventManager::Get()->RaiseEvent(make_shared<SceneChangedEvent>(scene_id), this);
	}

	SceneManager* SceneManager::Instance = nullptr;
}
