# cppgamelib
This is a simple game development library for c++.
It depends on TinyXML2 and SDL
To usem you'll need to link to it - it is a shared library. (cppgamelib.lib)

![Basic Architecture](AppGameLibArchitecture.png)

## Functionality
## Assets

The `Asset` class is an object that holds a path to an asset file, like an image file or sound file and then is able to load that asset into memory. 

Each asset has an associated `Uid` that distinguishes this asset from others. Each asset can be either of type (Graphic, Sprite, Audio, Font) and additionally can be associated with a particular scene via the `SceneId`

There are a variety of built-in assets that derive from Asset and therefore have the overriding ability to Load() and Unload() themselves into/from memory.

1. AudioAsset - loads/unloads and holds a reference to an audio clip that is used in a scene
2. FontAsset - loads/unloads and holds a reference to a font that is used in a scene
3. GraphicAsset - loads/unloads and holds a reference to an image that is used in the scene
4. SpriteAsset - loads/unloads and holds a reference to a sprite-sheet that is used in the scene.

## Event management

### EventManager
The `EventManager` is the central component of the event management system. It is a Singleton which allows supported objects (`IEventSubscriber`) to subscribe and raise `Events`. 

When an event is subscribed to, it is stored in the EventManager's event queue and when an Event is raised that matches the subscribed event, the object that subscribed to the event gets notified. 

All objects that can subscribe and be notified need to inherit from `IEventSubscriber` as this is the interface that the EventManager will use to contact it.

An `Event` can be subscribed to by asking the EventManager to subscribe to it and passing yourself as a reference so it can get back to you to notify when the event is raised.

Each event has a particular it to its particular `EventId` that is associated with a known event type, and when they are raised the data associated with the event is also stored in the Event object that is returned to the subscriber. 

### IEventSubscriber

All objects that are IEventSubscribers can raise events with the EventManager and typically do so by calling the `RaiseEvent()` function. Also, they can use the `Subscribe()` to subscribe to an event. Internally this calls the EventManager.

### Built-in Events

#### PlayerMovedEvent

This Event is raised when the code that registers a up/down/left/right controller press is detected. Typically this is subscribed to by the `Player` so that the player can move itself.

#### GameObjectEvent

This Event is always associated with a particular `GameObject` and serves as a convenient way to raise events about them. The specific thing that the event relates to is encoded in the Event's `Context` member which is of type `GameObjectEventContext` and currently specifies two contexts:

1. Remove
2. RemoveSubscription

#### SceneChangedEvent

This Event is raised when a particular scene is being changed, typically in response to a new level starting where a scene can be interpreted as a level. The event carries with it the `SceneId` that was changed to. This is used by the `ResourceManager` to unload resources that are not for that scene and load resources that are.

#### SceneLoadedEvent 

This event is raised when the scene is completely loaded, i.e the resources for that scene have all been loaded into memory and theoretically, that scene can be presented/drawn.

#### StartNetworkLevelEvent

This event is raised when its time to start a level in a networked game. The idea is that the event is propagated to all players who have joined a network game. It contains which level they should all load.

#### UpdateAllGameObjectsEvent

This event is raised in the game loop when objects so can update themselves. 

#### NetworkPlayerJoinedEventId

This event is raised when a network player joins.

#### NetworkTrafficReceivedEvent

This event is raised when network traffic is received. 

#### AddGameObjectToCurrentSceneEvent

This event is raised when an object needs to be added to the scene. This is typically subscribed to by the `SceneManager`. 

#### LevelChangedEventTypeEventId
TBD

## Resource Management
### ResourceManager

The ResourceManager will read and index all the assets that are defined in the global resources file. It will also ask assets that are associated with a particular scene to load themselves when it receives the `SceneChangedEvent`. Equally in handling this event, it unloads any assets that are now not associated with the current scene. You should call `ResourceManager::IndexResourceFile()` before using or depending on this behaviour.

The `ResourceManager` keeps a list of all the resources by keeping a list of `Asset`s (see Assets)

The `ResourceManager` can also be consulted to return information about the assets in the resources file, as it effectively manages the resources specified therein. 

`ResourceManager::GetAssetInfo()` can be used to obtain the asset associated with a specific asset Id or asset name. You can also query the number of loaded or unloaded resources (`GetCountLoadedResources()` and `GetCountUnloadedResources()` respectively ) and get the total number of resources that the resource manager knows about.

## Scene Management
### SceneManager

  - Audio (via SDL2)
  - Font managment (via SDL2)
  - 2D Drawing (via SDL2)
  - Logging (limited)
## Dependencies:
 ### Include
 C:\SDL\SDL2\include 
 C:\SDL\SDL2_image-2.0.4\include 
 C:\SDL\SDL2_mixer-devel-2.0.4-VC\SDL2_mixer-2.0.4\include 
 C:\SDL\SDL2_ttf-2.0.15\include 
 C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include
 ### Link
 C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86  
 C:\SDL\SDL2\lib\x86 
 ### libs
 SDL2.lib
 SDL2main.lib
 SDL2_image.lib
 Winmm.lib
 SDL2_mixer.lib
 d3d10.lib
 d3dx10.lib 
 SDL2_ttf.lib
