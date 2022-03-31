#include "SceneLoadedEvent.h"
#include "Event.h"
using namespace std;
namespace gamelib
{
	SceneLoadedEvent::SceneLoadedEvent(int scene_id): Event(EventType::scene_loaded), scene_id_(scene_id)
	{
	}

	string SceneLoadedEvent::ToString()
	{
		return "SceneLoadedEvent";
	}
}