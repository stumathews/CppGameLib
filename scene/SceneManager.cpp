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

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Create a Scene Manager.
	/// <remarks>A scene is a collection of objects. A scene is loaded from a file</remarks>
	/// </summary>
	SceneManager::SceneManager(GameWorldData& gameWorld,std::string sceneFolder) : sceneFolder(sceneFolder), gameWorld(gameWorld)
	{ }

	/// <summary>
	/// Initialize the scene manager
	/// </summary>
	/// <returns>true if done successfully, false otherwise</returns>
	bool SceneManager::Initialize()
	{
		isInitialized = LogThis("SceneManager::initialize()", SettingsManager::Get()->get_bool("global", "verbose"), [&]()
		{
			// I care about when the level changes
			EventManager::Get()->SubscribeToEvent(EventType::LevelChangedEventType, this);

			// I care about when I'm asked to add game object to current scene
			EventManager::Get()->SubscribeToEvent(EventType::AddGameObjectToCurrentScene, this);
			EventManager::Get()->SubscribeToEvent(EventType::GenerateNewLevel, this);
			EventManager::Get()->SubscribeToEvent(EventType::GameObject, this);

			return true;
		}, true, true);
		return isInitialized;
	}

	/// <summary>
	/// Emit event to switch to scene 1 on initialization of scene manager.
	/// </summary>
	/// <param name="scene_id">Scene to start</param>
	void SceneManager::StartScene(int scene_id)
	{		
		// This triggers usually the ResourceManager that loads the resources in for the scene (see ResourceManager::process_events etc..)
		EventManager::Get()->RaiseEvent(std::make_shared<SceneChangedEvent>(scene_id), this);
	}

	/// <summary>
	/// Handle Scene Manager events
	/// </summary>
	vector<shared_ptr<Event>> SceneManager::HandleEvent(const std::shared_ptr<Event> event)
	{
		vector<shared_ptr<Event>> secondaryEvents;

		switch(event->type)
		{
			case EventType::PositionChangeEventType: 
				break;			
			case EventType::LevelChangedEventType: 

				// load in new scene
				LoadNewScene(event);			
				break;
			case EventType::DoLogicUpdateEventType:
				break;	
			case EventType::AddGameObjectToCurrentScene:  
				{ // new scope
					// add new object to scene (last layer)
					const auto gameObject = std::dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(event)->GetGameObject();
					AddGameObjectToScene(gameObject);
				}
				break;
			case EventType::PlayerMovedEventType: 
				break;
			case EventType::scene_loaded: 
				break;
			case EventType::GameObject:
				const auto gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(event);

				// Inspect the GameObject Event context for details on what to do with this GameObject
				if (gameObjectEvent->context == GameObjectEventContext::Remove)
				{
					RemoveGameObjectFromLayers(gameObjectEvent->gameObject->id);
				}
				break;
		}
		
		// We dont generate any events yet;
		return secondaryEvents;
	}

	/// <summary>
	/// Remove game Object from all Layers
	/// </summary>
	/// <param name="GameObjectId">gameObject to remove</param>
	void SceneManager::RemoveGameObjectFromLayers(int GameObjectId)
	{
		// Remove from each layer, the occurnace of the object denoted by gameObjectId
		for_each(begin(layers), end(layers), [&GameObjectId](Layer layer)
		{
			// Look for game object that match incomming id
			auto result = std::remove_if(begin(layer.layerObjects), end(layer.layerObjects), [=](weak_ptr<GameObject> gameObject)
			{
				if (auto ptr = gameObject.lock())
				{
					return ptr->id == GameObjectId;
				}
				return false;
			});			
		});
	}

	/// <summary>
	/// Load new scene from scene file
	/// </summary>
	void SceneManager::LoadNewScene(const std::shared_ptr<Event>& event)
	{
		// This will raise the event 
		auto raiseSceneLoadedEventFunction = [this](int scene_id, const string& scene_name)
		{
			LogMessage("Scene "+ to_string(scene_id) +" : "+ scene_name +" loaded.");
			EventManager::Get()->RaiseEvent(make_unique<SceneLoadedEvent>(scene_id), this);
		};

		string scene_name;
		
		const auto scene = std::dynamic_pointer_cast<SceneChangedEvent>(event)->scene_id;

		switch(scene)
		{
		case 1:			
			scene_name = sceneFolder + "scene1.xml";
			break;
		case 2:
			scene_name = sceneFolder + "scene2.xml";
			break;
		case 3:
			scene_name = sceneFolder +"scene3.xml";
			break;
		case 4:
			scene_name = sceneFolder + "scene4.xml";
			break;
		default:
			scene_name = sceneFolder + "scene1.xml";
		}

		if(!scene_name.empty())
		{
			try 
			{
				bool success = ReadSceneFile(scene_name);
				if (success)
				{
					raiseSceneLoadedEventFunction(scene, scene_name);
				}
			} catch(exception &e)
			{
				THROW(13, string("Cloud not load scene file: ") + string(e.what()), GetSubscriberName());
			}
		}
	}

	/// <summary>
	/// Add game object to scene
	/// </summary>
	/// <param name="game_object"></param>
	void SceneManager::AddGameObjectToScene(const std::shared_ptr<GameObject>& game_object)
	{
		// add to last layer of the scene
		layers.back().layerObjects.push_back(game_object);
	}

	/// <summary>
	/// Add new layer to scene
	/// </summary>
	Layer& SceneManager::AddLayer(const std::string& name)
	{
		auto layer = Layer();
		layer.name = name;
		layers.push_back(layer);

		return layer;
	}

	/// <summary>
	/// Find layer by name
	/// </summary>
	Layer& SceneManager::FindLayer(const std::string& name)
	{
		for (auto& layer : layers)
		{
			if (layer.name == name)
			{
				return layer;
			}
		}
		
		THROW(12, string("layer not found:" + name), GetSubscriberName());
	}

	/// <summary>
	/// Remove layer by name
	/// </summary>
	void SceneManager::RemoveLayer(const std::string& name)
	{
		std::remove_if(begin(layers), end(layers), [&name](Layer& layer) 
		{ 
			return layer.name._Equal(name); 
		});
	}

	/// <summary>
	/// Compare layer order
	/// </summary>
	bool CompareLayerOrder(const Layer& rhs, const Layer& lhs)
	{
		return lhs.zorder < rhs.zorder;
	}

	/// <summary>
	/// Sort layers
	/// </summary>
	void SceneManager::SortLayers()
	{
		layers.sort(CompareLayerOrder);
	}

	/// <summary>
	/// Update scene manager
	/// </summary>
	void SceneManager::Update()
	{
		// Scene manager does not need updating
	}

	/// <summary>
	/// Gte Scene layers
	/// </summary>
	std::list<Layer> SceneManager::GetLayers() const
	{
		return layers;
	}

	/// <summary>
	/// Read scene
	/// </summary>
	/// <param name="filename">Scene file</param>
	bool SceneManager::ReadSceneFile(const std::string& filename)
	{
		if(currentSceneName == filename)
		{
			LogMessage(string("Scene already loaded. Skipping."));
			return true;
		}

		LogMessage("Loading scene: " + string(filename));
		
		/* Eg. A Scene is composed of a) resources at various positions and scene as a visibility setting

		<scene id="2">
		  <layer name="layer0" posx="0" posy="0" visible="true">
			<objects>
			  <object posx="100" posy="40" resourceId="7" visible="true" colourKey="true" r="0" g="0" b="0"></object>
			  <object posx="500" posy="40" resourceId="8" visible="true" colourKey="true" r="0" g="0" b="0"></object>
			</objects>
		  </layer>
		</scene>

		Read in the scene details and store them in the scene manager
		*/

		tinyxml2::XMLDocument doc;		

		// Load file
		doc.LoadFile(filename.c_str());

		// Start parsing scene file
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
						auto layer = Layer();
						layer.zorder = layers.size();					

						// Loop through layer attributes
						for(auto layerAttributes = layerElement->FirstAttribute(); layerAttributes; layerAttributes = layerAttributes->Next()) // // <layer name="layer0" posx="0" posy="0" visible="true"
						{						
							// populate the new layer object:
							
							const string name(layerAttributes->Name());
							const string value(layerAttributes->Value());						

							if(name == "name")
							{
								OnNameParse(layer, value);
								continue;
							}

							if(name == "posx")
							{
								OnPosXParse(layer, value);
								continue;
							}

							if(name == "posy") 
							{
								OnPosYParse(layer, value);
								continue;
							}

							if (name == "visible")
							{
								OnVisibleParse(layer, value);
								continue;
							}
						}

						// Process inner contents of the layer 
						for(auto layerContent = layerNode->FirstChild(); layerContent; layerContent = layerContent->NextSibling()) // <object ...
						{
							// We have an object in the layer
							if(std::string(layerContent->Value()) == "objects") 
							{
								for(auto objectNode = layerContent->FirstChild(); objectNode; objectNode = objectNode->NextSibling())
								{	
									auto objectElement = objectNode->ToElement();

									if(objectElement == nullptr)
									{
										Logger::Get()->LogThis(string("Invalid or null object found in scene file"));
										continue;
									}

									// Build game object
									auto gameObject = GameObjectFactory::GetInstance().BuildGameObject(objectElement);
									
									// We have a new game object guys!
									gameWorld.objects.push_back(gameObject);

									// Add game object to this layer
									layer.layerObjects.push_back(gameObject);																
								}
							}
						}

						// Add this layer to scene
						layers.push_back(layer);
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

	void SceneManager::OnVisibleParse(Layer& layer, const std::string& value)
	{
		layer.visible = (value == "true") ? true : false;
	}

	void SceneManager::OnPosYParse(Layer& layer, const std::string& value)
	{
		layer.y = static_cast<int>(std::atoi(value.c_str()));
	}

	void SceneManager::OnPosXParse(Layer& layer, const std::string& value)
	{
		layer.x = static_cast<int>(std::atoi(value.c_str()));
	}

	void SceneManager::OnNameParse(Layer& layer, const std::string& value)
	{
		layer.name = value;
	}

	string SceneManager::GetSubscriberName()
	{
		return "SceneManager";
	}

}
