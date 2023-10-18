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

The `EventManager` is the central component of the event management system. It is a Singleton which allows supported objects (`IEventSubscriber`) to subscribe and raise `Events`. 

When an event is subscribed to, it is stored in the EventManager's event queue and when an Event is raised that matches the subscribed event, the object that subscribed to the event gets notified. 

All objects that can subscribe and be notified need to inherit from `IEventSubscriber` as this is the interface that the EventManager will use to contact it.

An `Event` can be subscribed to by asking the EventManager to subscribe to it. Each event has a particular it to its particular `EventId` that is associated with a known event type, and when they are raised the data associated with the event is also stored in the Event object that is returned to the subscriber. 

  - Resource Managment
  - Scene Managment
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
